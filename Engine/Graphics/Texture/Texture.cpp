#include "Texture.h"
#include <ktx.h>
#include <Engine/Graphics/Vulkan/Utils/VulkanUtils.h>
#include <Debug/ErrorDialog.h>

void Texture::Create(const VulkanContext& context, const std::filesystem::path& path, const TextureCreateInfo& info) {

    m_device = context.device;
    m_physicalDevice = context.physicalDevice;
    m_commandPool = context.commandPool;
    m_graphicsQueue = context.graphicsQueue;
    m_info = info;
    m_path = path;

    // Przy recreate, reload mogą być wycieki dlatego tu:
    Shutdown();

    // Wczytuje plik KTX2
    ktxTexture2* texture = nullptr;
    KTX_error_code result = ktxTexture2_CreateFromNamedFile(path.string().c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &texture);
    if (result != KTX_SUCCESS) {
        throw std::runtime_error("KTX texture create failed: " + path.string());
    }
    if (!texture) {
        throw std::runtime_error("KTX texture s null");
    }

    // Pobiera dane z pliku KTX2
    ktx_uint8_t* data = ktxTexture_GetData(reinterpret_cast<ktxTexture*>(texture));
    VkDeviceSize imageSize = ktxTexture_GetDataSize(reinterpret_cast<ktxTexture*>(texture));
    m_width = texture->baseWidth;
    m_height = texture->baseHeight;
    m_mipLevels = texture->numLevels;
    m_format = static_cast<VkFormat>(texture->vkFormat);

    // Wypisuje na konsolę informacje o teksturze
    std::cout << "TEXTURE INFO" << std::endl;
    std::cout << "Filename: " << path << std::endl;
    std::cout << "Width: " << m_width << std::endl;
    std::cout << "Height: " << m_height << std::endl;
    std::cout << "MipLevels: " << m_mipLevels << std::endl;
    std::cout << "ImageSize: " << imageSize << std::endl;
    std::cout << "Format: " << VkFormatToString(m_format) << std::endl;

    // Staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;
    CreateBuffer(m_device, m_physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

    void* mapped = nullptr;
    VK_CHECK(vkMapMemory(m_device, stagingMemory, 0, imageSize, 0, &mapped));
    memcpy(mapped, data, imageSize);
    vkUnmapMemory(m_device, stagingMemory);

    // Tworzenie obrazu VkImage
    CreateImage();

    // Layout: UNDEFINED -> TRANSFER_DST
    TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Kopiowanie mipmap
    std::vector<VkBufferImageCopy> regions;

    for (uint32_t i = 0; i < m_mipLevels; i++) {
        ktx_size_t offset;
        ktxTexture2_GetImageOffset(texture, i, 0, 0, &offset);

        VkBufferImageCopy region{};
        region.bufferOffset = offset;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = i;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width = std::max(1u, m_width >> i);
        region.imageExtent.height = std::max(1u, m_height >> i);
        region.imageExtent.depth = 1;
        regions.push_back(region);
    }

    CopyBufferToImage(stagingBuffer, m_image, regions);

    // Layout: TRANSFER_DST -> SHADER_READ
    TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // ImageView
    CreateImageView();

    // Sampler
    CreateSampler();

    // Cleanup staging + KTX
    vkDestroyBuffer(context.device, stagingBuffer, nullptr);
    vkFreeMemory(context.device, stagingMemory, nullptr);
    ktxTexture_Destroy(reinterpret_cast<ktxTexture*>(texture));

}

void Texture::Shutdown() {

    if (m_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(m_device, m_sampler, nullptr);
        m_sampler = VK_NULL_HANDLE;
    }

    if (m_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_device, m_imageView, nullptr);
        m_imageView = VK_NULL_HANDLE;
    }

    if (m_image != VK_NULL_HANDLE) {
        vkDestroyImage(m_device, m_image, nullptr);
        m_image = VK_NULL_HANDLE;
    }

    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

}


void Texture::CreateImage() {

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = m_width;
    imageInfo.extent.height = m_height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = m_mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateImage(m_device, &imageInfo, nullptr, &m_image));

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_device, m_image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        m_physicalDevice,
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_CHECK(vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory));

    VK_CHECK(vkBindImageMemory(m_device, m_image, m_memory, 0));

}

void Texture::TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout) {

    VkCommandBuffer cmd = BeginSingleTimeCommands(m_device, m_commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;

    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    } else {

        throw std::runtime_error("Texture '" + m_path.string() + "': unsupported layout transition");

    }

    vkCmdPipelineBarrier(cmd, srcStage, dstStage,0,0, nullptr,0, nullptr,1, &barrier);

    EndSingleTimeCommands(m_device, m_commandPool, m_graphicsQueue, cmd);

}

void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, const std::vector<VkBufferImageCopy>& regions) {

    VkCommandBuffer cmd = BeginSingleTimeCommands(m_device, m_commandPool);

    vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, static_cast<uint32_t>(regions.size()),regions.data());

    EndSingleTimeCommands(m_device, m_commandPool, m_graphicsQueue, cmd);

}

void Texture::CreateImageView() {

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_format;

    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = m_mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageView));

}

void Texture::CreateSampler() {

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = m_info.magFilter;
    samplerInfo.minFilter = m_info.minFilter;
    samplerInfo.addressModeU = m_info.addressMode;
    samplerInfo.addressModeV = m_info.addressMode;
    samplerInfo.addressModeW = m_info.addressMode;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(m_mipLevels);
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.mipmapMode = m_info.mipmapMode;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;

    VK_CHECK(vkCreateSampler(m_device, &samplerInfo, nullptr, &m_sampler));

}

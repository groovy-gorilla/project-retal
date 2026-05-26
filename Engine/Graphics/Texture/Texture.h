#pragma once

#include "pch.h"
#include "Graphics/Vulkan/VulkanRenderer.h"

struct TextureCreateInfo {
    VkFilter magFilter = VK_FILTER_LINEAR;
    VkFilter minFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
};

class Texture {
public:
    Texture() = default;
    ~Texture() = default;

    void Create(const VulkanContext& context, const std::filesystem::path& path, const TextureCreateInfo& info = {});

    void Shutdown();

    // Zakaz kopiowania
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    [[nodiscard]] VkImage GetImage() const { return m_image; }
    [[nodiscard]] VkImageView GetImageView() const { return m_imageView; }
    [[nodiscard]] VkSampler GetSampler() const { return m_sampler; }
    [[nodiscard]] VkFormat GetFormat() const { return m_format; }
    [[nodiscard]] uint32_t GetWidth() const { return m_width; }
    [[nodiscard]] uint32_t GetHeight() const { return m_height; }
    [[nodiscard]] uint32_t GetMipLevels() const { return m_mipLevels; }
    [[nodiscard]] const TextureCreateInfo& GetTextureCreateInfo() const { return m_info; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
    VkFormat m_format = VK_FORMAT_UNDEFINED;

    uint32_t m_width = 0;
    uint32_t m_height = 0;

    uint32_t m_mipLevels = 1;

    TextureCreateInfo m_info;
    std::filesystem::path m_path;

    void CreateImage();
    void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, const std::vector<VkBufferImageCopy>& regions);
    void CreateImageView();
    void CreateSampler();


};



// Font atlas
// Bitmaps
// Icons
// HUD
// Menus
// Skybox
// Logos
// Loading screens
// Sprites
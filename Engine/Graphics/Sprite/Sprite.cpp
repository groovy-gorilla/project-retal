#include "Sprite.h"

void Sprite::Create(VulkanContext context, const std::filesystem::path& path, uint32_t framesInFlight) {

    TextureCreateInfo info{};
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    m_texture.Create(context, path, info);

    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    bindings.push_back(binding);

    m_descriptor.Create(context.device, bindings, framesInFlight);

    for (uint32_t i = 0; i < framesInFlight; i++) {
        m_descriptor.UpdateTexture(i, 0, m_texture.GetImageView(), m_texture.GetSampler());
    }

}

void Sprite::Shutdown() {

    m_descriptor.Destroy();
    m_texture.Shutdown();

}

void Sprite::SetPosition(uint32_t width, uint32_t height) {

    m_position = fvec2(width, height);

}

void Sprite::SetSize(uint32_t width, uint32_t height) {

    m_size = fvec2(width, height);

}

const fvec2& Sprite::GetPosition() const {

    return m_position;

}

const fvec2& Sprite::GetSize() const {

    return m_size;

}


#include "Image.h"

void Image::Create(const VulkanContext& context, const std::filesystem::path& path) {

    TextureCreateInfo info{};
    info.magFilter = VK_FILTER_LINEAR;
    info.minFilter = VK_FILTER_LINEAR;
    info.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    m_texture.Create(context, path, info);

}

void Image::Shutdown() {

    m_texture.Shutdown();

}

void Image::SetPosition(const fvec2& position) {

    m_position = position;

}

void Image::SetSize(const fvec2& size) {

    m_size = size;

}

const fvec2& Image::GetPosition() const {

    return m_position;

}

const fvec2& Image::GetSize() const {

    return m_size;

}


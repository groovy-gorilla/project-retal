#include "Font.h"

void Font::Initialize(const VulkanContext& context, const std::filesystem::path& textureFontPath, const std::filesystem::path& dataFontPath) {

    TextureCreateInfo textureInfo {};
    textureInfo.magFilter = VK_FILTER_LINEAR;
    textureInfo.minFilter = VK_FILTER_LINEAR;
    textureInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    textureInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    m_texture.Create(context, textureFontPath, textureInfo);

    LoadFontData(dataFontPath);

}

void Font::Destroy() {

    m_texture.Shutdown();

}

void Font::LoadFontData(const std::filesystem::path& dataFontPath) {

    // Open the font data file.
    std::ifstream file(dataFontPath, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to load font data file: " + dataFontPath.string());
    }

    // First read atlas width.
    file.read(
        reinterpret_cast<char*>(&m_fontHeight),
        sizeof(m_fontHeight));

    // Second read font height.
    file.read(
        reinterpret_cast<char*>(&m_atlasWidth),
        sizeof(m_atlasWidth));

    // Read in the 95 used ascii characters for text.
    file.read(
        reinterpret_cast<char*>(m_fontData.data()),
        sizeof(FontType) * m_fontData.size());

    file.close();

}

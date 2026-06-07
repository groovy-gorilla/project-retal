#include "Font.h"

void Font::Initialize(const VulkanContext& context, const std::filesystem::path& textureFontPath, const std::filesystem::path& dataFontPath, TextureFilter filter, Settings& settings) {

    LoadAtlas(context, textureFontPath, filter);

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    std::vector<VkDescriptorSetLayoutBinding> bindings = {
        binding
    };

    m_descriptor.Create(context.device, bindings, settings.MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < settings.MAX_FRAMES_IN_FLIGHT; i++) {
        m_descriptor.UpdateTexture(i, 0, m_texture.GetImageView(), m_texture.GetSampler());
    }

    LoadFDA(dataFontPath);

}

void Font::Destroy() {

    m_texture.Shutdown();
    m_descriptor.Destroy();

}

void Font::LoadFDA(const std::filesystem::path& dataFontPath) {

    // Open the font data file.
    std::ifstream file(dataFontPath, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to load font data file: " + dataFontPath.string());
    }

    // Wczytuje nagłówek
    file.read(reinterpret_cast<char*>(&m_header), sizeof(FontHeader2));
    if (!file.good()) {
        throw std::runtime_error("Failed to read font file (" + dataFontPath.filename().string() + ")");
    }
    if (m_header.version != 1) {
        throw std::runtime_error("Unsupported font version (" + dataFontPath.filename().string() + ")");
    }

    // Czyta glify
    m_glyphs.resize(m_header.glyphCount);
    file.read(reinterpret_cast<char*>(m_glyphs.data()), sizeof(Glyph2) * m_glyphs.size());

}


void Font::LoadAtlas(const VulkanContext& context, const std::filesystem::path& textureFontPath, TextureFilter filter) {

    TextureCreateInfo textureInfo {};
    if (filter == TextureFilter::Linear) {
        textureInfo.magFilter = VK_FILTER_LINEAR;
        textureInfo.minFilter = VK_FILTER_LINEAR;
    } else {
        textureInfo.magFilter = VK_FILTER_NEAREST;
        textureInfo.minFilter = VK_FILTER_NEAREST;
    }
    textureInfo.addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    textureInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    m_texture.Create(context, textureFontPath, textureInfo);

}
#pragma once
#include <cstdint>

#include "Graphics/Texture/Texture.h"
#include "Graphics/Vulkan/VulkanRenderer.h"


struct FontHeader2 {
    uint32_t version;
    uint32_t atlasWidth;
    uint32_t atlasHeight;
    uint32_t fontSize;
    uint32_t glyphCount;
};

struct Glyph2 {
    uint32_t x;
    uint32_t width;
    uint32_t advance;
};

class Font {
public:
    void Initialize(
        const VulkanContext& context,
        const std::filesystem::path& textureFontPath,
        const std::filesystem::path& dataFontPath,
        TextureFilter filter,
        ApplicationDesc& desc);

    void Destroy();

    [[nodiscard]] const Texture& GetTexture() const { return m_texture; }
    [[nodiscard]] const Glyph2& GetCharacter(uint32_t unicode) const { return m_glyphs[unicode - 32]; }
    [[nodiscard]] float GetFontSize() const { return m_header.fontSize; }
    [[nodiscard]] uint32_t GetAtlasWidth() const { return m_header.atlasWidth; }
    [[nodiscard]] uint32_t GetAtlasHeight() const { return m_header.atlasHeight; }
    [[nodiscard]] VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) { return m_descriptor.GetSet(frameIndex); }
    [[nodiscard]] const VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const { return m_descriptor.GetSet(frameIndex); }

private:
    std::vector<Glyph2> m_glyphs = {};
    FontHeader2 m_header = {};
    Texture m_texture;
    Descriptor m_descriptor;

    void LoadFDA(const std::filesystem::path& dataFontPath);
    void LoadAtlas(const VulkanContext& context, const std::filesystem::path& textureFontPath, TextureFilter filter);

};
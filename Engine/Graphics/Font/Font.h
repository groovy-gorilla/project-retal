#pragma once
#include <string>

#include "Graphics/Texture/Texture.h"

struct FontHeader {
    uint32_t version;
    uint32_t atlasWidth;
    uint32_t atlasHeight;
    float lineHeight;
    float ascender;
    float descender;
    uint32_t glyphCount;
};

struct Glyph {
    uint32_t unicode;
    float advance;
    float atlasLeft;
    float atlasBottom;
    float atlasRight;
    float atlasTop;
    float planeLeft;
    float planeBottom;
    float planeRight;
    float planeTop;
};

class Font {
public:
    void Initialize(
        const VulkanContext& context,
        const std::filesystem::path& textureFontPath,
        const std::filesystem::path& dataFontPath,
        ApplicationDesc& desc);

    void Destroy();

    [[nodiscard]] const Texture& GetTexture() const { return m_texture; }
    [[nodiscard]] const Glyph& GetCharacter(uint32_t unicode) const { return m_glyphs[unicode - 32]; }
    [[nodiscard]] float GetLineHeight() const { return m_header.lineHeight; }
    [[nodiscard]] uint32_t GetAtlasWidth() const { return m_header.atlasWidth; }
    [[nodiscard]] uint32_t GetAtlasHeight() const { return m_header.atlasHeight; }
    [[nodiscard]] VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) { return m_descriptor.GetSet(frameIndex); }
    [[nodiscard]] const VkDescriptorSet GetDescriptorSet(uint32_t frameIndex) const { return m_descriptor.GetSet(frameIndex); }
    [[nodiscard]] float GetAscender() const { return m_header.ascender; }
    [[nodiscard]] float GetDescender() const { return m_header.descender; }

private:
    std::vector<Glyph> m_glyphs = {};
    FontHeader m_header = {};
    Texture m_texture;
    Descriptor m_descriptor;

    void LoadFDA(const std::filesystem::path& dataFontPath);
    void LoadAtlas(const VulkanContext& context, const std::filesystem::path& textureFontPath);

};
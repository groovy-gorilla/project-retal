#pragma once
#include <string>

#include "Graphics/Texture/Texture.h"

struct FontType {
    uint16_t left;
    uint16_t size;
};

class Font {
public:
    void Initialize(
        const VulkanContext& context,
        const std::filesystem::path& textureFontPath,
        const std::filesystem::path& dataFontPath);

    void Destroy();

    [[nodiscard]] const Texture& GetTexture() const { return m_texture; }
    [[nodiscard]] const FontType& GetCharacter(char character) const { return m_fontData[character - 32]; }


private:
    std::array<FontType, 95> m_fontData;
    Texture m_texture;
    uint32_t m_atlasWidth = 0;
    uint32_t m_fontHeight = 0;

    void LoadFontData(const std::filesystem::path& dataFontPath);

};

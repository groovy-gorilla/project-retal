#include "Text.h"

#include <cassert>

void Text::Initialize(const VulkanContext& context, Font& font, uint32_t maxCharacters) {

    m_font = &font;
    m_context = context;
    m_maxCharacters = maxCharacters;

    VkDeviceSize vertexSize =
        sizeof(TextVertex) *
        4 *
        m_maxCharacters;

    VkDeviceSize indexSize =
        sizeof(uint32_t) *
        6 *
        m_maxCharacters;

    m_vertexBuffer.Create(
        m_context.device,
        m_context.physicalDevice,
        vertexSize,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    m_indexBuffer.Create(
        m_context.device,
        m_context.physicalDevice,
        indexSize,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

}

void Text::Destroy() {

    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();

}

void Text::SetText(const std::string& text) {

    if (m_text == text) return;

    m_text = text;

    BuildGeometry();

    m_dirty = true;

    m_indexCount = static_cast<uint32_t>(m_indices.size());

    Update();

}

void Text::SetPosition(uint32_t width, uint32_t height) {

    m_position.x = width;
    m_position.y = height;

}

void Text::SetSize(float size) {

    m_scale = size / m_font->GetFontSize();

}

void Text::SetColor(const lina::fvec4& color) {

    m_color = color;

}

void Text::Update() {

    if (!m_dirty) return;

    UpdateBuffers();

    m_dirty = false;

}

void Text::SetCharacterSpacing(float spacing) {

    m_characterSpacing = spacing;

}

void Text::SetLineSpacing(float spacing) {

    m_lineSpacing = spacing;

}

void Text::BuildGeometry() {

    m_vertices.clear();
    m_indices.clear();

    float penX = 0.0f;
    float penY = 0.0f;

    for (char character : m_text) {

        if(character == '\n') {
            penX = 0.0f;
            penY += (m_font->GetFontSize() + m_lineSpacing) * m_scale;
            continue;
        }

        const Glyph2& glyph = m_font->GetCharacter(character);

        if (glyph.width == 0) {
            penX += (glyph.advance + m_characterSpacing) * m_scale;
            continue;
        }

        float left = penX;
        float right = penX + glyph.width * m_scale;
        float bottom = penY;
        float top = penY + static_cast<float>(m_font->GetAtlasHeight()) * m_scale;

        float u0 = glyph.x / static_cast<float>(m_font->GetAtlasWidth());
        float u1 = (glyph.x + glyph.width) / static_cast<float>(m_font->GetAtlasWidth());

        float v0 = 0.0f;
        float v1 = 1.0f;

        uint32_t base = static_cast<uint32_t>(m_vertices.size());

        m_vertices.push_back({{left, bottom, 0.0f}, {u0, v0}});
        m_vertices.push_back({{right, bottom, 0.0f}, {u1, v0}});
        m_vertices.push_back({{right, top, 0.0f}, {u1, v1}});
        m_vertices.push_back({{left, top, 0.0f}, {u0, v1}});

        m_indices.push_back(base + 0);
        m_indices.push_back(base + 2);
        m_indices.push_back(base + 1);

        m_indices.push_back(base + 2);
        m_indices.push_back(base + 0);
        m_indices.push_back(base + 3);

        penX += (glyph.advance + m_characterSpacing) * m_scale;

    }

}

void Text::UpdateBuffers() {

    if (m_vertices.empty()) return;

    VkDeviceSize vertexSize = sizeof(TextVertex) * m_vertices.size();
    VkDeviceSize indexSize = sizeof(uint32_t) * m_indices.size();

    assert(m_vertices.size() <= m_maxCharacters * 4);
    assert(m_indices.size() <= m_maxCharacters * 6);

    void* vertexData = m_vertexBuffer.Map();
    memcpy(vertexData, m_vertices.data(), vertexSize);
    m_vertexBuffer.Unmap();

    void* indexData = m_indexBuffer.Map();
    memcpy(indexData, m_indices.data(), indexSize);
    m_indexBuffer.Unmap();

}
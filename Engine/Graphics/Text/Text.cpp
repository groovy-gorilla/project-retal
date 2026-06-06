#include "Text.h"

void Text::Initialize(const VulkanContext& context, Font& font) {

    m_font = &font;
    m_context = context;

}

void Text::Destroy() {

    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();

}

void Text::SetText(const std::string& text) {

    m_text = text;

    BuildGeometry();
    UpdateBuffers();

    m_indexCount = static_cast<uint32_t>(m_indices.size());

}

void Text::SetPosition(uint32_t width, uint32_t height) {

    m_position.x = width;
    m_position.y = height;

}

void Text::SetScale(float scale) {

    m_scale = scale;

}

void Text::SetColor(const lina::fvec4& color) {

    m_color = color;

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

    for(char character : m_text) {

        if(character == '\n') {
            penX = 0.0f;
            penY -= (m_font->GetLineHeight() + m_lineSpacing) * m_scale;
            continue;
        }

        const Glyph& glyph = m_font->GetCharacter(character);

        if (character == ' ') {
            penX += (glyph.advance + m_characterSpacing) * m_scale;
            continue;
        }

        float left = penX + glyph.planeLeft * m_scale;
        float right = penX + glyph.planeRight * m_scale;
        float top = penY - glyph.planeBottom * m_scale;   // Odwracamy top i bottom
        float bottom = penY - glyph.planeTop * m_scale;

        float u0 = glyph.atlasLeft / static_cast<float>(m_font->GetAtlasWidth());
        float v0 = 1.0f - glyph.atlasBottom / static_cast<float>(m_font->GetAtlasHeight());
        float u1 = glyph.atlasRight / static_cast<float>(m_font->GetAtlasWidth());
        float v1 = 1.0f - glyph.atlasTop / static_cast<float>(m_font->GetAtlasHeight());

        uint32_t base = static_cast<uint32_t>(m_vertices.size());

        m_vertices.push_back({{left, bottom, 0.0f}, {u0, v1}});
        m_vertices.push_back({{right, bottom, 0.0f}, {u1, v1}});
        m_vertices.push_back({{right, top, 0.0f}, {u1, v0}});
        m_vertices.push_back({{left, top, 0.0f}, {u0, v0}});

        m_indices.push_back(base + 0);
        m_indices.push_back(base + 1);
        m_indices.push_back(base + 2);

        m_indices.push_back(base + 2);
        m_indices.push_back(base + 3);
        m_indices.push_back(base + 0);

        penX += (glyph.advance + m_characterSpacing) * m_scale;

    }

}

void Text::UpdateBuffers() {

    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();

    if (m_vertices.empty()) return;

    VkDeviceSize vertexSize = sizeof(TextVertex) * m_vertices.size();
    VkDeviceSize indexSize = sizeof(uint32_t) * m_indices.size();

    m_vertexBuffer.Create(m_context.device, m_context.physicalDevice, vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_indexBuffer.Create(m_context.device, m_context.physicalDevice, indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* vertexData = m_vertexBuffer.Map();
    memcpy(vertexData, m_vertices.data(), vertexSize);
    m_vertexBuffer.Unmap();

    void* indexData = m_indexBuffer.Map();
    memcpy(indexData, m_indices.data(), indexSize);
    m_indexBuffer.Unmap();

}

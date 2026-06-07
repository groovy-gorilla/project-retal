#pragma once
#include <string>
#include "Engine/Graphics/Font/Font.h"
#include "Graphics/Vulkan/Wrappers/Buffer.h"
#include "Math/Lina64.h"

struct TextVertex {
    lina::fvec3 position;
    lina::fvec2 uv;
};

class Text {
public:
    void Initialize(const VulkanContext& context, Font& font);
    void Destroy();

    void SetText(const std::string& text);
    void SetPosition(uint32_t width, uint32_t height);
    void SetSize(float size);
    void SetColor(const lina::fvec4& color);

    void SetCharacterSpacing(float spacing);
    void SetLineSpacing(float spacing);

    [[nodiscard]] Font& GetFont() const { return *m_font; }

    [[nodiscard]] const std::string& GetText() const { return m_text; }

    [[nodiscard]] const lina::fvec2& GetPosition() const { return m_position; }
    [[nodiscard]] float GetScale() const { return m_scale; }

    [[nodiscard]] const lina::fvec4& GetColor() const { return m_color; }

    [[nodiscard]] float GetCharacterSpacing() const { return m_characterSpacing; }
    [[nodiscard]] float GetLineSpacing() const { return m_lineSpacing; }

    [[nodiscard]] const std::vector<TextVertex>& GetVertices() const { return m_vertices; }
    [[nodiscard]] const std::vector<uint32_t>& GetIndices() const { return m_indices; }

    [[nodiscard]] const Buffer& GetVertexBuffer() const { return m_vertexBuffer; }
    [[nodiscard]] const Buffer& GetIndexBuffer() const { return m_indexBuffer; }
    [[nodiscard]] uint32_t GetIndexCount() const { return m_indexCount; }

private:
    VulkanContext m_context;
    Font* m_font = nullptr;
    std::string m_text;
    float m_characterSpacing = 0.0f;
    float m_lineSpacing = 0.0f;
    lina::fvec2 m_position = { 0.0f, 0.0f };
    lina::fvec4 m_color = {1,1,1,1};
    float m_scale = 1.0f;

    std::vector<TextVertex> m_vertices;
    std::vector<uint32_t> m_indices;

    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    uint32_t m_indexCount = 0;

    void BuildGeometry();
    void UpdateBuffers();

};
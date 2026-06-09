#include "Model.h"

void Model::CreateCube(VkDevice device, VkPhysicalDevice physicalDevice) {

    std::vector<Vertex> vertices = {

        // Front
        {{-0.5f, -0.5f,  0.5f}, {1,0,0}},
        {{ 0.5f, -0.5f,  0.5f}, {1,0,0}},
        {{ 0.5f,  0.5f,  0.5f}, {1,0,0}},
        {{-0.5f,  0.5f,  0.5f}, {1,0,0}},

        // Back
        {{ 0.5f, -0.5f, -0.5f}, {0,1,0}},
        {{-0.5f, -0.5f, -0.5f}, {0,1,0}},
        {{-0.5f,  0.5f, -0.5f}, {0,1,0}},
        {{ 0.5f,  0.5f, -0.5f}, {0,1,0}},

        // Left
        {{-0.5f, -0.5f, -0.5f}, {0,0,1}},
        {{-0.5f, -0.5f,  0.5f}, {0,0,1}},
        {{-0.5f,  0.5f,  0.5f}, {0,0,1}},
        {{-0.5f,  0.5f, -0.5f}, {0,0,1}},

        // Right
        {{ 0.5f, -0.5f,  0.5f}, {1,1,0}},
        {{ 0.5f, -0.5f, -0.5f}, {1,1,0}},
        {{ 0.5f,  0.5f, -0.5f}, {1,1,0}},
        {{ 0.5f,  0.5f,  0.5f}, {1,1,0}},

        // Top
        {{-0.5f,  0.5f,  0.5f}, {1,0,1}},
        {{ 0.5f,  0.5f,  0.5f}, {1,0,1}},
        {{ 0.5f,  0.5f, -0.5f}, {1,0,1}},
        {{-0.5f,  0.5f, -0.5f}, {1,0,1}},

        // Bottom
        {{-0.5f, -0.5f, -0.5f}, {0,1,1}},
        {{ 0.5f, -0.5f, -0.5f}, {0,1,1}},
        {{ 0.5f, -0.5f,  0.5f}, {0,1,1}},
        {{-0.5f, -0.5f,  0.5f}, {0,1,1}}
    };

    std::vector<uint32_t> indices = {

        0,1,2, 2,3,0,        // Front
        4,5,6, 6,7,4,        // Back
        8,9,10, 10,11,8,     // Left
        12,13,14, 14,15,12,  // Right
        16,17,18, 18,19,16,  // Top
        20,21,22, 22,23,20   // Bottom
    };

    m_indexCount = static_cast<uint32_t>(indices.size());

    m_vertexBuffer.Create(
        device,
        physicalDevice,
        sizeof(Vertex) * vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    auto* vertexData = static_cast<Vertex*>(m_vertexBuffer.Map());
    memcpy(vertexData, vertices.data(), sizeof(Vertex) * vertices.size());
    m_vertexBuffer.Unmap();

    m_indexBuffer.Create(
        device,
        physicalDevice,
        sizeof(uint32_t) * indices.size(),
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    auto* indexData = static_cast<uint32_t*>(m_indexBuffer.Map());
    memcpy(indexData,indices.data(), sizeof(uint32_t) * indices.size());
    m_indexBuffer.Unmap();

    m_indexCount = static_cast<uint32_t>(indices.size());

}

void Model::Destroy() {
    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();
    m_indexCount = 0;
}


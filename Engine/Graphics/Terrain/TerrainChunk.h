#pragma once

#include "Graphics/Vulkan/Wrappers/Buffer.h"
#include "Math/Lina64.h"

struct ChunkHeader {
    uint32_t vertexCount;
    uint32_t indexCount;
};

struct ChunkVertex {
    lina::fvec3 position;
    lina::fvec3 normal;
    lina::fvec3 color;
};

class TerrainChunk {

public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        const std::filesystem::path& path
    );

    void Destroy();

    Buffer& GetVertexBuffer() { return m_vertexBuffer; }
    Buffer& GetIndexBuffer() { return m_indexBuffer; }
    uint32_t GetIndexCount() const { return m_indexCount; }

private:
    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    uint32_t m_indexCount = 0;

};
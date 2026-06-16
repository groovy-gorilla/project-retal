#include "TerrainChunk.h"

#include <cfloat>

void TerrainChunk::Create(VkDevice device, VkPhysicalDevice physicalDevice, const std::filesystem::path& path) {

    std::ifstream in(path, std::ios::binary);

    if (!in) {
        throw std::runtime_error("Cannot open chunk file: " + path.string());
    }

    ChunkHeader header{};

    in.read(reinterpret_cast<char*>(&header), sizeof(ChunkHeader));

    std::vector<ChunkVertex> vertices(header.vertexCount);
    std::vector<uint32_t> indices(header.indexCount);

    in.read(reinterpret_cast<char*>(vertices.data()), vertices.size() * sizeof(ChunkVertex));
    in.read(reinterpret_cast<char*>(indices.data()), indices.size() * sizeof(uint32_t));

    in.close();

    m_vertexBuffer.Create(device, physicalDevice, sizeof(ChunkVertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto* vertexData = static_cast<ChunkVertex*>(m_vertexBuffer.Map());
    memcpy(vertexData, vertices.data(), sizeof(ChunkVertex) * vertices.size());
    m_vertexBuffer.Unmap();

    m_indexBuffer.Create(device, physicalDevice, sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto* indexData = static_cast<uint32_t*>(m_indexBuffer.Map());
    memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
    m_indexBuffer.Unmap();

    m_indexCount = static_cast<uint32_t>(indices.size());

}

void TerrainChunk::Destroy() {

    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();

    m_indexCount = 0;

}
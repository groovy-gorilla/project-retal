#pragma once

#include "../TerrainChunk/TerrainChunk.h"
#include "Math/Lina64.h"

struct TerrainChunkInstance {
    TerrainChunk* chunk;
    lina::fvec3 position;
    float rotation;
};

enum class TerrainPreset {
    SOMALIA,
    COLUMBIA,
    LIBIA,
    YUGOSLAVIA,
    SEA // sprawdź te nazwy
};

class Terrain {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        TerrainPreset region);

    void Destroy();

    std::vector<TerrainChunk> GetChunks() const { return m_chunks; }
    std::vector<TerrainChunkInstance> GetInstances() const { return m_instances; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    std::vector<TerrainChunk> m_chunks;
    std::vector<TerrainChunkInstance> m_instances;

    void LoadChunks(TerrainPreset region);
    void Generate(uint32_t width, uint32_t height);

};
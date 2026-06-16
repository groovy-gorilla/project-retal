#pragma once

#include "../TerrainChunk/TerrainChunk.h"
#include "Math/Lina64.h"

struct TerrainChunkInstance {
    TerrainChunk* chunk;
    lina::vec2 position;
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

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;

    std::vector<TerrainChunk> m_chunks;

    void LoadChunks(TerrainPreset region);

};
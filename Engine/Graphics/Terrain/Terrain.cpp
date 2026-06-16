#include "Terrain.h"

void Terrain::Create(VkDevice device, VkPhysicalDevice physicalDevice, TerrainPreset region) {

    m_device = device;
    m_physicalDevice = physicalDevice;

    LoadChunks(region);

}

void Terrain::LoadChunks(TerrainPreset region) {

    if (region == TerrainPreset::SOMALIA) {

        TerrainChunk chunk;

        chunk.Create(m_device, m_physicalDevice, "Models/plane.imodel");
        m_chunks.push_back(chunk);

    }

}

void Terrain::Destroy() {

    for (auto& chunk : m_chunks) {
        chunk.Destroy();
    }

    m_chunks.clear();

}


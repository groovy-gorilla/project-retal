#include "Terrain.h"

#include <random>

void Terrain::Create(VkDevice device, VkPhysicalDevice physicalDevice, TerrainPreset region) {

    m_device = device;
    m_physicalDevice = physicalDevice;

    LoadChunks(region);
    Generate(35, 35);

}

void Terrain::LoadChunks(TerrainPreset region) {

    if (region == TerrainPreset::SOMALIA) {

        TerrainChunk chunk;

        chunk.Create(m_device, m_physicalDevice, "Models/fields_0.imodel");
        m_chunks.push_back(chunk);

        chunk.Create(m_device, m_physicalDevice, "Models/fields_1.imodel");
        m_chunks.push_back(chunk);

    }

}

void Terrain::Destroy() {

    for (auto& chunk : m_chunks) {
        chunk.Destroy();
    }

    m_chunks.clear();

}

void Terrain::Generate(uint32_t width, uint32_t height) {

    m_instances.clear();

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<uint32_t> chunkDist(
        0,
        static_cast<uint32_t>(m_chunks.size() - 1)
    );

    std::uniform_int_distribution<uint32_t> rotationDist(0, 3);

    const float chunkSize = 5000.0;

    for (uint32_t z = 0; z < height; z++)
    {
        for (uint32_t x = 0; x < width; x++)
        {
            TerrainChunkInstance instance{};

            instance.chunk = &m_chunks[chunkDist(gen)];

            instance.position = {
                x * chunkSize,
                0.0f,
                z * chunkSize
            };

            instance.rotation = static_cast<float>(rotationDist(gen) * 90);

            m_instances.push_back(instance);
        }
    }

}
#include "Skydome.h"

void Skydome::Create(VkDevice device, VkPhysicalDevice physicalDevice) {

    const std::string filename = "Models/skydome.imodel";

    std::ifstream in(filename, std::ios::binary);

    if (!in) {
        throw std::runtime_error("Cannot open skydome file: " + filename);
    }

    Header header{};

    in.read(reinterpret_cast<char*>(&header), sizeof(Header));

    std::vector<SkydomeVertex> vertices(header.vertexCount);
    std::vector<uint32_t> indices(header.indexCount);

    in.read(reinterpret_cast<char*>(vertices.data()), vertices.size() * sizeof(SkydomeVertex));
    in.read(reinterpret_cast<char*>(indices.data()), indices.size() * sizeof(uint32_t));

    in.close();

    m_vertexBuffer.Create(device, physicalDevice, sizeof(SkydomeVertex) * vertices.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto* vertexData = static_cast<SkydomeVertex*>(m_vertexBuffer.Map());
    memcpy(vertexData, vertices.data(), sizeof(SkydomeVertex) * vertices.size());
    m_vertexBuffer.Unmap();

    m_indexBuffer.Create(device, physicalDevice, sizeof(uint32_t) * indices.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    auto* indexData = static_cast<uint32_t*>(m_indexBuffer.Map());
    memcpy(indexData, indices.data(), sizeof(uint32_t) * indices.size());
    m_indexBuffer.Unmap();

    m_indexCount = static_cast<uint32_t>(indices.size());
}

void Skydome::Destroy() {

    m_vertexBuffer.Destroy();
    m_indexBuffer.Destroy();

    m_indexCount = 0;

}

void Skydome::HorizonBroadening(float altitude) {

    // ROZSZERZANIE HORYZONTU WRAZ Z WYSOKOŚCIĄ
    float t = std::clamp(altitude / 15000.0f, 0.0f, 1.0f);
    m_skyColors.groundHorizon.w = lina::Lerp(-0.001f, -0.25f, t);

    // OPUSZCZANIE HORYZONTU WRAZ Z WYSOKOŚCIĄ
    m_skyColors.horizonOffset = lina::Lerp(0.0f, 0.08f, t);

}

void Skydome::SetSkyColors(SkyPreset preset) {

    if (preset == SkyPreset::SOMALIA_DAWN) {
        m_skyColors.zenith          = {0.30, 0.31, 0.52, 1.0 };
        m_skyColors.sky             = {0.30, 0.31, 0.52, 0.423 };
        m_skyColors.horizon         = {0.57, 0.49, 0.38, 0.0 };
        m_skyColors.groundHorizon   = {0.36, 0.33, 0.33, -0.005 };
        m_skyColors.ground          = {0.34, 0.31, 0.30, -1.0 };
    }

}


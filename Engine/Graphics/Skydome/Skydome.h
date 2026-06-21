#pragma once

#include "Graphics/Vulkan/Wrappers/Buffer.h"
#include "Math/Lina64.h"

struct SkydomeHeader {
    uint32_t vertexCount;
    uint32_t indexCount;
};

struct SkydomeVertex {
    float x;
    float y;
    float z;
};

struct SkyColors {  // DOMYŚLNIE SOMALIA DAWN
    lina::fvec4 zenith          = {};
    lina::fvec4 sky             = {};
    lina::fvec4 upperHorizon    = {};
    lina::fvec4 horizon         = {};
    lina::fvec4 groundHorizon   = {};
    lina::fvec4 ground          = {};

    float horizonOffset         = 0.0f;

    float pad0;
    float pad1;
    float pad2;
};

enum class SkyPreset {
    SOMALIA_DAWN,
    SOMALIA_DAYTIME_CLEAR_SKY,
    SOMALIA_DAYTIME_CLOUD,
    SOMALIA_DAYTIME_STORM,
    SOMALIA_NIGHTTIME_CLEAR_SKY,
    SOMALIA_NIGHTTIME_CLOUD
};

class Skydome {
public:

    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice
    );

    void Destroy();

    void SetSkyColors(SkyPreset preset);
    SkyColors& GetSkyColors() { return m_skyColors; }
    const SkyColors& GetSkyColors() const { return m_skyColors; }

    Buffer& GetVertexBuffer() { return m_vertexBuffer; }
    Buffer& GetIndexBuffer() { return m_indexBuffer; }
    uint32_t GetIndexCount() const { return m_indexCount; }

    void HorizonBroadening(float altitude);

private:

    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;

    uint32_t m_indexCount = 0;

    SkyColors m_skyColors;

};
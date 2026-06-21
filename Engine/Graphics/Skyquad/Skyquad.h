#pragma once

#include "Math/Lina64.h"

struct SkyquadColors {
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

enum class SkyquadPreset {
    SOMALIA_DAWN,
    SOMALIA_DAYTIME_CLEAR_SKY,
    SOMALIA_DAYTIME_CLOUD,
    SOMALIA_DAYTIME_STORM,
    SOMALIA_NIGHTTIME_CLEAR_SKY,
    SOMALIA_NIGHTTIME_CLOUD
};

class Skyquad {
public:

    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice
    );

    void Destroy();

    void SetSkyColors(SkyquadPreset preset);
    SkyquadColors& GetSkyColors() { return m_skyColors; }
    const SkyquadColors& GetSkyColors() const { return m_skyColors; }

    void HorizonBroadening(float altitude);

private:

    SkyquadColors m_skyColors = {};

};
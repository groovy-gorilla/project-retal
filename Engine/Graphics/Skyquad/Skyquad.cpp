#include "Skyquad.h"

void Skyquad::Create(VkDevice device, VkPhysicalDevice physicalDevice) {



}

void Skyquad::Destroy() {



}

void Skyquad::HorizonBroadening(float altitude) {

    // ROZSZERZANIE HORYZONTU WRAZ Z WYSOKOŚCIĄ
    float t = std::clamp(altitude / 15000.0f, 0.0f, 1.0f);
    m_skyColors.groundHorizon.w = lina::Lerp(-0.001f, -0.01f, t);

    // OPUSZCZANIE HORYZONTU WRAZ Z WYSOKOŚCIĄ
    m_skyColors.horizonOffset = lina::Lerp(0.0f, 0.08f, t);

}

void Skyquad::SetSkyColors(SkyquadPreset preset) {

    if (preset == SkyquadPreset::SOMALIA_DAWN) {
        m_skyColors.zenith          = {0.302, 0.318, 0.525, 1.0 };
        m_skyColors.sky             = {0.302, 0.318, 0.525, 0.5 };
        m_skyColors.upperHorizon    = {0.475, 0.475, 0.475, 0.01 };
        m_skyColors.horizon         = {0.573, 0.490, 0.380, 0.0 };
        m_skyColors.groundHorizon   = {0.443, 0.412, 0.365, -0.01 };
        m_skyColors.ground          = {0.349, 0.318, 0.302, -0.5 };
    }

}


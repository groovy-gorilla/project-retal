#pragma once

#include <vulkan/vulkan.h>

enum class AntiAliasing {
    None,
    MSAA,
    SSAA,
    MSAA_TAA,
    SSAA_TAA
};

struct ApplicationDesc {

    const char* title                    = "Indigo Engine";

    uint32_t width                      = 800;
    uint32_t height                     = 600;

    float scaling                       = 1.0f;

    bool fullscreen                     = true;
    bool aspectRatio                    = false;
    bool vsync                          = true;

    const uint32_t maxFramesInFlight    = 2;
    VkFilter filter                     = VK_FILTER_NEAREST;

    // AntiAliasing
    AntiAliasing aaMode                  = AntiAliasing::SSAA;

    // MSAA
    VkSampleCountFlagBits msaaSamples   = VK_SAMPLE_COUNT_1_BIT;

    // SSAA
    float ssaaScale                     = 1.0f;     // scale choice: 1.0 - 1.25 - 1.5 - 2.0

};
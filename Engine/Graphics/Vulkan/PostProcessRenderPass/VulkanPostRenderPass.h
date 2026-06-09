#pragma once

#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

struct Settings;

struct PostPushConstants {
    int hdrEnable;
    float exposure;
    int dithering;
};

class VulkanPostRenderPass {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        Settings& settings);

    void Destroy(VkDevice device);

    void Render(
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        RenderTarget& inputColor,
        VkExtent2D extent,
        Settings& settings,
        float exposure);

    RenderTarget& GetColor() { return m_color; }

private:

    // PIPELINE
    Pipeline m_pipeline;

    // ATTACHMENTS
    RenderTarget m_color;

    // DESCRIPTOR
    Descriptor m_descriptor;

};



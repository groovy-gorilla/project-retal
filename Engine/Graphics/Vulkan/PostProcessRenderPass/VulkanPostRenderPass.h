#pragma once

#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

struct ApplicationDesc;

struct PostPushConstants {
    int hdrEnable;
    float exposure;
    int dithering;
};

class VulkanPostRenderPass {
public:
    void Create(
        VkDevice device,
        VkExtent2D extent,
        VkFormat swapchainFormat,
        ApplicationDesc desc);

    void Destroy(VkDevice device);

    void Render(
        VkDevice device,
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        RenderTarget& inputColor,
        VkFramebuffer framebuffer,
        VkExtent2D extent,
        ApplicationDesc& desc,
        float exposure);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] Descriptor GetDescriptor() const { return m_descriptor; }

private:

    // PASS
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    // PIPELINE
    Pipeline m_pipeline;

    // DESCRIPTOR
    Descriptor m_descriptor;

};



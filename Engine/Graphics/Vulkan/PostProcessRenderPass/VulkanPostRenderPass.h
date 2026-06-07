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

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] Descriptor GetDescriptor() const { return m_descriptor; }
    RenderTarget& GetColor() { return m_color; }

private:

    // PASS
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    // PIPELINE
    Pipeline m_pipeline;

    // ATTACHMENTS
    RenderTarget m_color;

    // DESCRIPTOR
    Descriptor m_descriptor;

    // FRAMEBUFFER
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

};



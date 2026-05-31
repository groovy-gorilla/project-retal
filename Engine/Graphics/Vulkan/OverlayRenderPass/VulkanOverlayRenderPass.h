#pragma once

#include "Core/ApplicationDesc.h"
#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

class VulkanOverlayRenderPass {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        ApplicationDesc& desc);

    void Destroy(VkDevice device);

    void Begin(
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        RenderTarget& inputColor,
        VkExtent2D extent,
        ApplicationDesc& desc);

    void End(VkCommandBuffer commandBuffer);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] Descriptor GetDescriptor() const { return m_descriptor; }
    RenderTarget& GetColor() { return m_color; }

private:

    // DEVICE HANDLE
    VkDevice m_device = VK_NULL_HANDLE;

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



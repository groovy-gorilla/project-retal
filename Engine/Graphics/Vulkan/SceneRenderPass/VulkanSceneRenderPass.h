#pragma once

#include "Core/ApplicationDesc.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

class VulkanSceneRenderPass {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        VkFormat depthFormat,
        AntiAliasing aaMode,
        VkSampleCountFlagBits samples,
        ApplicationDesc& desc);

    void Destroy(VkDevice device);

    void Begin(VkCommandBuffer commandBuffer);

    void End(VkCommandBuffer commandBuffer);

    void PipelineBind(VkCommandBuffer commandBuffer);

    VkExtent2D GetExtent() const { return m_renderExtent; }

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }

    RenderTarget& GetColor() { return m_color; }
    RenderTarget& GetDepth() { return m_depth; }
    RenderTarget& GetResolve() { return m_resolve; }


private:

    // PASS
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    // PIPELINE
    Pipeline m_pipeline;

    // ATTACHMENTS
    RenderTarget m_color;
    RenderTarget m_depth;
    RenderTarget m_resolve;

    // FRAMEBUFFER
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;

    VkExtent2D m_renderExtent = {};

};


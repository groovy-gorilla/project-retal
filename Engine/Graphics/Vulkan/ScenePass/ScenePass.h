#pragma once

#include "Core/Settings.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

class ScenePass {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        VkFormat depthFormat,
        AntiAliasing aaMode,
        VkSampleCountFlagBits samples,
        Settings& settings);

    void Destroy(VkDevice device);

    void Begin(VkCommandBuffer commandBuffer);

    void End(VkCommandBuffer commandBuffer);

    void PipelineBind(VkCommandBuffer commandBuffer);

    VkExtent2D GetExtent() const { return m_renderExtent; }

    RenderTarget& GetColor() { return m_color; }
    RenderTarget& GetDepth() { return m_depth; }
    RenderTarget& GetResolve() { return m_resolve; }


private:

    // PIPELINE
    Pipeline m_pipeline;

    // ATTACHMENTS
    RenderTarget m_color;
    RenderTarget m_depth;
    RenderTarget m_resolve;

    VkExtent2D m_renderExtent = {};

};


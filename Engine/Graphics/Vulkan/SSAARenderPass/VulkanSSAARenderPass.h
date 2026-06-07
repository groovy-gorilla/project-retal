#pragma once

#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"
#include "Graphics/Vulkan/Wrappers/Descriptor.h"

struct Settings;

class VulkanSSAARenderPass {
public:

    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        RenderTarget& sceneColor,
        Settings& settings);

    void Destroy(VkDevice device);

    void Render(
        VkCommandBuffer commandBuffer,
        VkExtent2D extent,
        uint32_t currentFrame);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] Descriptor GetDescriptor() const { return m_descriptor; }

    RenderTarget& GetColor() { return m_color; }

private:

    //PASS
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



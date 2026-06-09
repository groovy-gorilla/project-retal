#pragma once

#include "Core/Settings.h"
#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

class VulkanPresentRenderPass {
public:
    void Create(
        VkDevice device,
        VkFormat swapchainFormat,
        Settings& settings);

    void Destroy();

    void Render(
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        RenderTarget& inputColor,
        VkImage swapchainImage,
        VkImageView swapchainView,
        VkExtent2D extent,
        Settings& settings);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] Descriptor GetDescriptor() const { return m_descriptor; }

private:

    // DEVICE HANDLE
    VkDevice m_device = VK_NULL_HANDLE;

    // PASS
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    // PIPELINE
    Pipeline m_pipeline;

    // DESCRIPTOR
    Descriptor m_descriptor;

};



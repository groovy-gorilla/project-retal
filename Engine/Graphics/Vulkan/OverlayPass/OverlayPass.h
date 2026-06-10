#pragma once

#include "Core/Settings.h"
#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

class OverlayPass {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        Settings& settings);

    void Destroy(VkDevice device);

    void Begin(
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        RenderTarget& inputColor,
        VkExtent2D extent,
        Settings& settings);

    void End(VkCommandBuffer commandBuffer);

    Descriptor GetDescriptor() const { return m_descriptor; }
    RenderTarget& GetColor() { return m_color; }

private:

    // DEVICE HANDLE
    VkDevice m_device = VK_NULL_HANDLE;

    // PIPELINE
    Pipeline m_pipeline;

    // ATTACHMENTS
    RenderTarget m_color;

    // DESCRIPTOR
    Descriptor m_descriptor;

};



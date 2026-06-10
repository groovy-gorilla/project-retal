#pragma once

#include "Core/Settings.h"
#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

class PresentPass {
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

private:

    // DEVICE HANDLE
    VkDevice m_device = VK_NULL_HANDLE;

    // PIPELINE
    Pipeline m_pipeline;

    // DESCRIPTOR
    Descriptor m_descriptor;

};



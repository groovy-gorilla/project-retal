#pragma once

#include <vulkan/vulkan.h>

class VulkanQueues {
public:

    void Create(
        VkDevice device,
        uint32_t graphicsQueueFamily,
        uint32_t presentQueueFamily);

    VkQueue GetGraphics() const { return m_graphicsQueue; }
    VkQueue GetPresent() const { return m_presentQueue; }

private:
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;

};
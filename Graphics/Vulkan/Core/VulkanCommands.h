#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class VulkanCommands {
public:

    void Create(
        VkDevice device,
        uint32_t graphicsQueueFamily,
        uint32_t imageCount);

    void Destroy(VkDevice device);

    VkCommandBuffer Get(uint32_t index) const { return m_commandBuffers[index]; }

private:

    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;

};

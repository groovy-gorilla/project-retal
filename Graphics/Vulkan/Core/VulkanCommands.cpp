#include "VulkanCommands.h"

#include <iostream>

void VulkanCommands::Create(VkDevice device, uint32_t graphicsQueueFamily, uint32_t imageCount) {

    // COMMAND POOL
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = graphicsQueueFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool);

    // COMMAND BUFFERS
    m_commandBuffers.resize(imageCount);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = imageCount;

    vkAllocateCommandBuffers(device, &allocInfo, m_commandBuffers.data());

    std::cout << "[Vulkan] Commands created" << std::endl;

}

void VulkanCommands::Destroy(VkDevice device) {

    if (m_commandPool) {
        vkDestroyCommandPool(device, m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Commands destroyed" << std::endl;
    }

}
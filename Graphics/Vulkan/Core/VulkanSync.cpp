#include "VulkanSync.h"

#include <iostream>
#include <ostream>
#include "../../../Engine/Core/Error/ErrorDialog.h"

void VulkanSync::Create(VkDevice device) {

    // FENCE
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &m_renderFence));

    std::cout << "[Vulkan] Fence created" << std::endl;

    // SEMAPHORES
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphore));
    VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphore));

    std::cout << "[Vulkan] Semaphores created" << std::endl;

}

void VulkanSync::Destroy(VkDevice device) {

    if (m_renderFence) {
        vkDestroyFence(device, m_renderFence, nullptr);
        m_renderFence = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Fence destroyed" << std::endl;
    }

    if (m_imageAvailableSemaphore) {
        vkDestroySemaphore(device, m_imageAvailableSemaphore, nullptr);
        m_imageAvailableSemaphore = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Available sempahore destroyed" << std::endl;
    }

    if (m_renderFinishedSemaphore) {
        vkDestroySemaphore(device, m_renderFinishedSemaphore, nullptr);
        m_renderFinishedSemaphore = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Finished sempahore destroyed" << std::endl;
    }

}

void VulkanSync::Wait(VkDevice device) {
    VK_CHECK(vkWaitForFences(device, 1, &m_renderFence, VK_TRUE, UINT64_MAX));
}

void VulkanSync::Reset(VkDevice device) {
    VK_CHECK(vkResetFences(device, 1, &m_renderFence));
}
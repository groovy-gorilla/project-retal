#pragma once

#include <vulkan/vulkan.h>

class VulkanSync {
public:

    void Create(VkDevice device);
    void Destroy(VkDevice device);
    void Wait(VkDevice device);
    void Reset(VkDevice device);

    VkSemaphore GetImageAvailableSemaphore() const { return m_imageAvailableSemaphore; }
    VkSemaphore GetRenderFinishedSemaphore() const { return m_renderFinishedSemaphore; }
    VkFence GetFence() const { return m_renderFence; }

private:
    VkFence m_renderFence = VK_NULL_HANDLE;
    VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_renderFinishedSemaphore = VK_NULL_HANDLE;

};
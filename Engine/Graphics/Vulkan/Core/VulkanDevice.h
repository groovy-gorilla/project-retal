#pragma once

class VulkanDevice {
public:
    void Create(VkPhysicalDevice physicalDevice, uint32_t graphicsQueueFamily, uint32_t presentQueueFamily);
    void Destroy();

    [[nodiscard]] VkDevice Get() const { return m_device; }
    [[nodiscard]] VkQueue GetGraphicsQueue() const { return m_graphicsQueue; }
    [[nodiscard]] VkQueue GetPresentQueue() const { return m_presentQueue; }

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;


};
#pragma once

#include "pch.h"

class Buffer {
public:

    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties);

    void Destroy();

    void* Map() const;
    void Unmap() const;

    VkBuffer Get() const { return m_buffer; }
    VkDeviceMemory GetMemory() const { return m_memory; }
    VkDeviceSize GetSize() const { return m_size; }


private:

    VkDevice m_device = VK_NULL_HANDLE;

    VkBuffer m_buffer = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;

    VkDeviceSize m_size = 0;

};
#include "Buffer.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Engine/Debug/ErrorDialog.h"

void Buffer::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {

    m_device = device;
    m_size = size;

    // BUFFER
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = m_size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(m_device, &bufferInfo, nullptr, &m_buffer));

    // MEMORY REQUIREMENTS
    VkMemoryRequirements memRequirements{};
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memRequirements);

    // ALLOC
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    VK_CHECK(vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory));

    // BIND
    VK_CHECK(vkBindBufferMemory(m_device, m_buffer, m_memory, 0));

}

void Buffer::Destroy() {

    if (m_memory != VK_NULL_HANDLE) {
        vkFreeMemory(m_device, m_memory, nullptr);
        m_memory = VK_NULL_HANDLE;
    }

    if (m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_device, m_buffer, nullptr);
        m_buffer = VK_NULL_HANDLE;
    }

    m_size = 0;

}

void* Buffer::Map() const {
    void* data = nullptr;
    vkMapMemory(m_device, m_memory, 0, m_size, 0, &data);
    return data;
}

void Buffer::Unmap() const {
    vkUnmapMemory(m_device, m_memory);
}

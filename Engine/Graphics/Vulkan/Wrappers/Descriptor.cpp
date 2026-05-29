#include "pch.h"
#include "Descriptor.h"
#include "Debug/ErrorDialog.h"

void Descriptor::Create(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings, uint32_t maxFramesInFlight) {

    m_device = device;

    // LAYOUT
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_layout));

    // POOL SIZES
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (const auto& binding : bindings) {
        VkDescriptorPoolSize poolSize{};
        poolSize.type = binding.descriptorType;
        poolSize.descriptorCount = binding.descriptorCount * maxFramesInFlight;
        poolSizes.push_back(poolSize);
    }

    // POOL
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxFramesInFlight;

    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pool));

    // DESCRIPTOR SETS
    std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, m_layout);

    m_sets.resize(maxFramesInFlight);

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_pool;
    allocInfo.descriptorSetCount = maxFramesInFlight;
    allocInfo.pSetLayouts = layouts.data();

    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, m_sets.data()));

}

void Descriptor::Destroy() {

    m_sets.clear();

    if (m_pool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_pool, nullptr);
        m_pool = VK_NULL_HANDLE;
    }

    if (m_layout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_layout, nullptr);
        m_layout = VK_NULL_HANDLE;
    }

    m_device = VK_NULL_HANDLE;

}

void Descriptor::UpdateUniformBuffer(uint32_t frameIndex, uint32_t binding, VkBuffer buffer, VkDeviceSize size) {

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = size;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_sets[frameIndex];
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);

}

void Descriptor::UpdateStorageBuffer(uint32_t frameIndex, uint32_t binding, VkBuffer buffer, VkDeviceSize size) {

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = size;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_sets[frameIndex];
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);

}

void Descriptor::UpdateTexture(uint32_t frameIndex, uint32_t binding, VkImageView imageView, VkSampler sampler) {

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_sets[frameIndex];
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);

}

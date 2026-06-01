#pragma once

enum class TextureFilter;
class RenderTarget;

class Descriptor {
public:

    void Create(
        VkDevice device,
        const std::vector<VkDescriptorSetLayoutBinding>& bindings,
        uint32_t maxFramesInFlight);

    void Destroy();

    [[nodiscard]] VkDescriptorSetLayout GetLayout() const { return m_layout; }
    [[nodiscard]] VkDescriptorPool GetPool() const { return m_pool; }
    [[nodiscard]] VkDescriptorSet GetSet(uint32_t frameIndex) const { return m_sets[frameIndex]; }

    VkDescriptorSetLayout* GetLayoutHandle() { return &m_layout; }
    VkDescriptorPool* GetPoolHandle() { return &m_pool; }
    VkDescriptorSet* GetSetHandle(uint32_t frameIndex) { return &m_sets[frameIndex]; }

    void UpdateUniformBuffer(
        uint32_t frameIndex,
        uint32_t binding,
        VkBuffer buffer,
        VkDeviceSize size);

    void UpdateStorageBuffer(
        uint32_t frameIndex,
        uint32_t binding,
        VkBuffer buffer,
        VkDeviceSize size);

    void UpdateTexture(
        uint32_t frameIndex,
        uint32_t binding,
        VkImageView imageView,
        VkSampler sampler);


private:

    VkDevice m_device = VK_NULL_HANDLE;

    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_sets;

};
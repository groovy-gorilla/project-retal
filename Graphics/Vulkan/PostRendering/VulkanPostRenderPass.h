#pragma once

#include <vulkan/vulkan.h>

class VulkanPostRenderPass {
public:
    void Create(
        VkDevice device,
        VkExtent2D extent,
        VkFormat swapchainFormat);

    void Destroy(VkDevice device);

    void Render(
        VkCommandBuffer commandBuffer,
        VkFramebuffer framebuffer,
        VkExtent2D extent);

    VkRenderPass Get() const { return m_renderPass; }

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;

};


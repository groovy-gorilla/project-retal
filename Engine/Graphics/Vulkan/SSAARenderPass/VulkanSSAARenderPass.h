#pragma once

#include "Graphics/Vulkan/Wrappers/RenderTarget.h"
#include "Graphics/Vulkan/Wrappers/VulkanTextureDescriptor.h"

struct ApplicationDesc;

class VulkanSSAARenderPass {
public:

    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkExtent2D renderExtent,
        VkFormat colorFormat,
        RenderTarget& sceneColor,
        RenderTarget& sceneDepth,
        ApplicationDesc& desc);

    void Destroy(VkDevice device);

    void Render(
        VkCommandBuffer commandBuffer,
        VkExtent2D extent,
        uint32_t currentFrame);

    [[nodiscard]] VkRenderPass Get() const { return m_renderPass; }
    [[nodiscard]] VulkanTextureDescriptor GetDescriptor() const { return m_sceneDescriptor; }

    RenderTarget& GetColor() { return m_color; }

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VulkanTextureDescriptor m_sceneDescriptor;

    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    void CreateFramebuffer(VkDevice device, VkExtent2D extent);

    RenderTarget m_color;

};



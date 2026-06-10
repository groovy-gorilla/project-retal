#pragma once

#include <vulkan/vulkan.h>

#include "Graphics/Vulkan/Wrappers/Descriptor.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

class Settings;

class SMAAPass {
public:

    void Create(VkDevice device,
                VkPhysicalDevice physicalDevice,
                VkExtent2D renderExtent,
                VkFormat colorFormat,
                Settings& settings,
                VkCommandPool commandPool,
                VkQueue graphicsQueue);

    void Render(VkCommandBuffer commandBuffer,
                VkExtent2D extent,
                uint32_t currentFrame,
                RenderTarget& inputColor);

    void Destroy(VkDevice device);

    RenderTarget& GetColor() { return m_color; }

private:

    // INTERNAL RENDERING
    void RenderEdgePass(VkCommandBuffer commandBuffer,
                        VkExtent2D extent,
                        uint32_t currentFrame);

    void RenderBlendPass(VkCommandBuffer commandBuffer,
                         VkExtent2D extent,
                         uint32_t currentFrame);

    void RenderNeighborhoodPass(VkCommandBuffer commandBuffer,
                                VkExtent2D extent,
                                uint32_t currentFrame);

    // EDGE PASS
    void CreateEdgeRenderPass(VkDevice device);

    void CreateEdgeFramebuffer(VkDevice device,
                               VkExtent2D extent);

    void CreateEdgeDescriptors(VkDevice device,
                               Settings& settings);

    void CreateEdgePipeline(VkDevice device,
                            VkExtent2D extent);

    // BLEND PASS
    void CreateBlendRenderPass(VkDevice device);

    void CreateBlendFramebuffer(VkDevice device,
                                VkExtent2D extent);

    void CreateBlendDescriptors(VkDevice device,
                                 Settings& settings);

    void CreateBlendPipeline(VkDevice device,
                             VkExtent2D extent);

    // NEIGHBORHOOD PASS
    void CreateNeighborhoodRenderPass(VkDevice device, RenderTarget& outputColor);

    void CreateNeighborhoodFramebuffer(VkDevice device,
                                       VkExtent2D extent,
                                       RenderTarget& outputColor);

    void CreateNeighborhoodDescriptors(VkDevice device,
                                       Settings& settings);

    void CreateNeighborhoodPipeline(VkDevice device,
                                    VkExtent2D extent);

private:

    // DEVICE
    VkDevice m_device = VK_NULL_HANDLE;

    // INTERMEDIATE TARGETS
    RenderTarget m_edgeColor{};
    RenderTarget m_blendColor{};

    // SMAA LOOKUP TEXTURES
    RenderTarget m_areaTexture{};
    RenderTarget m_searchTexture{};

    // EDGE PASS
    VkRenderPass m_edgeRenderPass = VK_NULL_HANDLE;
    VkFramebuffer m_edgeFramebuffer = VK_NULL_HANDLE;
    Descriptor m_edgeDescriptor{};
    VkDescriptorSetLayout m_edgeDescriptorLayout = VK_NULL_HANDLE;
    Pipeline m_edgePipeline;

    // BLEND PASS
    VkRenderPass m_blendRenderPass = VK_NULL_HANDLE;
    VkFramebuffer m_blendFramebuffer = VK_NULL_HANDLE;
    Descriptor m_blendDescriptor{};
    VkDescriptorSetLayout m_blendDescriptorLayout = VK_NULL_HANDLE;
    Pipeline m_blendPipeline;

    // NEIGHBORHOOD PASS
    VkRenderPass m_neighborhoodRenderPass = VK_NULL_HANDLE;
    VkFramebuffer m_neighborhoodFramebuffer = VK_NULL_HANDLE;
    Descriptor m_neighborhoodDescriptor{};
    VkDescriptorSetLayout m_neighborhoodDescriptorLayout = VK_NULL_HANDLE;
    Pipeline m_neighborhoodPipeline;

    VkCommandPool m_commandPool{};
    VkQueue m_graphicsQueue{};

    RenderTarget m_color;

};
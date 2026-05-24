#pragma once
#include "Graphics/Camera/Camera.h"
#include "Graphics/Resources/Image.h"

using namespace lina;

struct UIPushConstants {
    mat4 projection;
    fvec2 position;
    fvec2 size;
};

class ImageRenderer {
public:
    void Create(VkDevice device, VkRenderPass renderPass);
    void Shutdown();
    void Render(VkCommandBuffer commandBuffer, fvec2 screenSize, const Camera& camera);
    void SetImage(Image& image);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkCommandBuffer m_commandBuffer = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    Image* m_image = nullptr;

    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptor();
    void CreatePipelineLayout();
    void CreatePipeline();

};
#pragma once
#include "Graphics/Camera/Camera.h"

using namespace lina;

class Sprite;

struct UIPushConstants {
    fmat4 projection;
    fvec2 position;
    fvec2 size;
};

class SpriteRenderer {
public:
    void Create(VkDevice device, VkRenderPass renderPass);
    void Shutdown();
    void Render(VkCommandBuffer commandBuffer, VkExtent2D renderExtent, const Camera& camera);
    void SetSprite(Sprite& sprite);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    Sprite* m_sprite = nullptr;

    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void CreateDescriptorSet();
    void UpdateDescriptor();
    void CreatePipelineLayout();
    void CreatePipeline();

};
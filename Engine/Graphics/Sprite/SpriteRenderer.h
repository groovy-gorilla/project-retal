#pragma once
#include "Core/ApplicationDesc.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

using namespace lina;

class Sprite;

struct UIPushConstants {
    fmat4 projection;
    fvec2 position;
    fvec2 size;
};

class SpriteRenderer {
public:
    void Create(
        VkDevice device,
        VkRenderPass renderPass);

    void Shutdown();

    void Render(
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        const Sprite& sprite,
        const Camera& camera);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    Pipeline m_pipeline;
    VkDescriptorSetLayout m_descriptorLayout = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};
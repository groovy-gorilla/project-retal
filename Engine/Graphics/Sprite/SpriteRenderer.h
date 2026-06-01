#pragma once
#include "Core/ApplicationDesc.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Vulkan/Wrappers/Descriptor.h"
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
    void Create(VkDevice device, VkRenderPass renderPass, ApplicationDesc& desc);
    void Shutdown();
    void Render(uint32_t frameIndex, VkCommandBuffer commandBuffer, VkExtent2D renderExtent, const Camera& camera);
    void SetSprite(Sprite& sprite, ApplicationDesc& desc);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    Pipeline m_pipeline;
    Descriptor m_descriptor;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    Sprite* m_sprite = nullptr;

};
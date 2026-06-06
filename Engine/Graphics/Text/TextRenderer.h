#pragma once

#include "Graphics/Camera/Camera.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

class Text;

struct TextPushConstants {
    fmat4 projection;
    fvec4 position;
    fvec4 color;
};

class TextRenderer {
public:
    void Create(
        VkDevice device,
        VkRenderPass renderPass);

    void Shutdown();

    void Render(
        uint32_t frameIndex,
        VkCommandBuffer commandBuffer,
        const Text& text,
        const Camera& camera);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    Pipeline m_pipeline;
    VkDescriptorSetLayout m_descriptorLayout = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

};
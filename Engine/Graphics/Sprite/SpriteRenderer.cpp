#include "Graphics/Sprite/SpriteRenderer.h"

#include "Sprite.h"
#include "Debug/ErrorDialog.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"

void SpriteRenderer::Create(VkDevice device, VkRenderPass renderPass, ApplicationDesc& desc) {

    m_device = device;
    m_renderPass = renderPass;

    std::vector<VkDescriptorSetLayoutBinding> bindings{};
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(binding);

    m_descriptor.Create(device, bindings, desc.MAX_FRAMES_IN_FLIGHT);

    VkPushConstantRange pushConstants{};
    pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstants.offset = 0;
    pushConstants.size = sizeof(UIPushConstants);

    PipelineDesc pdesc;
    pdesc.renderPass = m_renderPass;
    pdesc.descriptorLayout = m_descriptor.GetLayout();
    pdesc.pushConstants = &pushConstants;
    pdesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Sprite/sprite_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Sprite/sprite_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = true;
    pdesc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    m_pipeline.Create(device, pdesc);

}

void SpriteRenderer::Shutdown() {

    vkDeviceWaitIdle(m_device);

    m_pipeline.Destroy(m_device);
    m_descriptor.Destroy();

}

void SpriteRenderer::Render(uint32_t frameIndex, VkCommandBuffer commandBuffer, VkExtent2D renderExtent, const Camera& camera) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    UIPushConstants push{};

    push.projection = ToFloat(camera.GetProjection());
    push.position = m_sprite->GetPosition();
    push.size = m_sprite->GetSize();

    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(UIPushConstants), &push);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, m_descriptor.GetSetHandle(frameIndex), 0, nullptr);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = renderExtent.width;
    viewport.height = renderExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = renderExtent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 4, 1, 0, 0);

}

void SpriteRenderer::SetSprite(Sprite& sprite, ApplicationDesc& desc) {

    m_sprite = &sprite;
    for (uint32_t i = 0; i < desc.MAX_FRAMES_IN_FLIGHT; i++) {
        m_descriptor.UpdateTexture(i, 0, m_sprite->GetTexture().GetImageView(), m_sprite->GetTexture().GetSampler());
    }
}


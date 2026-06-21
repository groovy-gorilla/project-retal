#include "Graphics/Sprite/SpriteRenderer.h"
#include "Debug/ErrorDialog.h"

#include "Sprite.h"

void SpriteRenderer::Create(VkDevice device, VkFormat colorFormat) {

    m_device = device;

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &binding;

    VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_descriptorLayout));

    VkPushConstantRange pushConstants{};
    pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstants.offset = 0;
    pushConstants.size = sizeof(SpritePushConstants);

    PipelineDesc pdesc;
    pdesc.descriptorLayout = m_descriptorLayout;
    pdesc.pushConstants = &pushConstants;
    pdesc.vertexShader = "../Engine/Graphics/Sprite/Shaders/sprite_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Sprite/Shaders/sprite_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = true;
    pdesc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    pdesc.cullMode = VK_CULL_MODE_BACK_BIT;
    pdesc.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pdesc.colorFormat = colorFormat;

    m_pipeline.Create(device, pdesc);

}

void SpriteRenderer::Shutdown() {

    vkDeviceWaitIdle(m_device);

    m_pipeline.Destroy(m_device);

    if (m_descriptorLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorLayout, nullptr);
        m_descriptorLayout = VK_NULL_HANDLE;
    }

}

void SpriteRenderer::Render(uint32_t frameIndex, VkCommandBuffer commandBuffer, const Sprite& sprite, const Camera& camera) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    SpritePushConstants push{};

    push.projection = ToFloat(camera.GetProjection());
    push.position = sprite.GetPosition();
    push.size = sprite.GetSize();

    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SpritePushConstants), &push);

    VkDescriptorSet set = sprite.GetDescriptorSet(frameIndex);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &set, 0, nullptr);

    vkCmdDraw(commandBuffer, 4, 1, 0, 0);

}



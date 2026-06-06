#include "Graphics/Text/TextRenderer.h"
#include "Debug/ErrorDialog.h"

#include "Text.h"

void TextRenderer::Create(VkDevice device, VkRenderPass renderPass) {

    m_device = device;
    m_renderPass = renderPass;

    VkVertexInputBindingDescription vertexBinding{};
    vertexBinding.binding = 0;
    vertexBinding.stride = sizeof(TextVertex);
    vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributes[2]{};
    attributes[0].binding = 0;
    attributes[0].location = 0;
    attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[0].offset = offsetof(TextVertex, position);
    attributes[1].binding = 0;
    attributes[1].location = 1;
    attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
    attributes[1].offset = offsetof(TextVertex, uv);

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
    pushConstants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;;
    pushConstants.offset = 0;
    pushConstants.size = sizeof(TextPushConstants);

    PipelineDesc pdesc;
    pdesc.renderPass = m_renderPass;
    pdesc.descriptorLayout = m_descriptorLayout;
    pdesc.pushConstants = &pushConstants;
    pdesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Text/text_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Text/text_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = true;
    pdesc.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pdesc.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    pdesc.cullMode = VK_CULL_MODE_NONE;
    pdesc.bindingDescription = &vertexBinding;
    pdesc.attributeDescriptions = attributes;
    pdesc.attributeCount = 2;

    m_pipeline.Create(device, pdesc);

}

void TextRenderer::Shutdown() {

    vkDeviceWaitIdle(m_device);

    m_pipeline.Destroy(m_device);

    if (m_descriptorLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorLayout, nullptr);
        m_descriptorLayout = VK_NULL_HANDLE;
    }

}

void TextRenderer::Render(uint32_t frameIndex, VkCommandBuffer commandBuffer, const Text& text, const Camera& camera) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    TextPushConstants push{};

    push.projection = ToFloat(camera.GetProjection());
    push.position = lina::fvec4(text.GetPosition().x, text.GetPosition().y, 0.0, 0.0);
    push.color = text.GetColor();

    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(TextPushConstants), &push);

    VkDescriptorSet set = text.GetFont().GetDescriptorSet(frameIndex);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &set, 0, nullptr);

    VkBuffer vertexBuffer = text.GetVertexBuffer().Get();
    VkDeviceSize offset = 0;

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

    vkCmdBindIndexBuffer(commandBuffer, text.GetIndexBuffer().Get(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, text.GetIndexCount(), 1, 0, 0,0);

}



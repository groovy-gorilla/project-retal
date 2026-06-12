#include "ModelRenderer.h"

#include "Core/Display.h"
#include "Graphics/Model/Model.h"
#include "Graphics/Camera/Camera.h"

void ModelRenderer::Create(VkDevice device, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples) {

    m_device = device;

    static VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    static VkVertexInputAttributeDescription attributes[2];

    attributes[0].binding = 0;
    attributes[0].location = 0;
    attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[0].offset = offsetof(Vertex, position);

    attributes[1].binding = 0;
    attributes[1].location = 1;
    attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributes[1].offset = offsetof(Vertex, color);

    static VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = sizeof(ModelPushConstants);

    m_pDesc.colorFormat = colorFormat;
    m_pDesc.depthFormat = depthFormat;
    m_pDesc.samples = samples;
    m_pDesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Model/model_vert.spv";
    m_pDesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Model/model_frag.spv";
    m_pDesc.bindingDescription = &binding;
    m_pDesc.attributeDescriptions = attributes;
    m_pDesc.attributeCount = 2;
    m_pDesc.depthTest = true;
    m_pDesc.depthWrite = true;
    m_pDesc.blending = false;
    m_pDesc.cullMode = VK_CULL_MODE_BACK_BIT;
    m_pDesc.frontFace = VK_FRONT_FACE_CLOCKWISE;
    m_pDesc.pushConstants = &push;

    m_pipeline.Create(device, m_pDesc);

}

void ModelRenderer::Destroy() {

    m_pipeline.Destroy(m_device);

}

void ModelRenderer::Render(VkCommandBuffer commandBuffer, Model& model, const Camera& camera) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    static float angle = 0.0f;
    angle += 0.01f;

    // MACIERZE
    fmat4 modelMatrix = RotateY(angle);
    fmat4 viewMatrix = ToFloat(camera.GetView());
    fmat4 projectionMatrix = ToFloat(camera.GetProjection());

    ModelPushConstants push{};
    push.mvp = projectionMatrix * viewMatrix * modelMatrix;

    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelPushConstants), &push);

    // ****************************

    VkBuffer vertexBuffer = model.GetVertexBuffer().Get();

    VkDeviceSize offset = 0;

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

    vkCmdBindIndexBuffer(commandBuffer, model.GetIndexBuffer().Get(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, model.GetIndexCount(), 1, 0, 0, 0);

}

void ModelRenderer::RecreatePipeline(VkDevice device, VkSampleCountFlagBits samples) {

    m_pipeline.Destroy(device);

    static VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    static VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = sizeof(ModelPushConstants);

    m_pDesc.samples = samples;
    m_pDesc.bindingDescription = &binding;
    m_pDesc.pushConstants = &push;

    m_pipeline.Create(device, m_pDesc);

}

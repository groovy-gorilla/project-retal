#include "SkydomeRenderer.h"

#include "Debug/ErrorDialog.h"
#include "Graphics/Skydome/Skydome.h"
#include "Graphics/Camera/Camera.h"

void SkydomeRenderer::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples) {

    m_device = device;

    // DESCRIPTOR SET LAYOUT
    VkDescriptorSetLayoutBinding slbinding{};
    slbinding.binding = 0;
    slbinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    slbinding.descriptorCount = 1;
    slbinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 1;
    info.pBindings = &slbinding;
    VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &m_descriptorLayout));

    // DESCRIPTOR POOL
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = 1;
    VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool));

    // DESCRIPTOR SET
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_descriptorLayout;
    VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSet));

    // BUFFER
    m_skyBuffer.Create(device, physicalDevice, sizeof(SkyColors), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_skyBuffer.Get();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(SkyColors);

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device,1, &write, 0, nullptr);



    static VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(SkydomeVertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    static VkVertexInputAttributeDescription attribute;
    attribute.binding = 0;
    attribute.location = 0;
    attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute.offset = offsetof(SkydomeVertex, x);

    static VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = sizeof(SkydomePushConstants);

    m_pDesc.colorFormat = colorFormat;
    m_pDesc.depthFormat = depthFormat;
    m_pDesc.samples = samples;
    m_pDesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Skydome/skydome_vert.spv";
    m_pDesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Skydome/skydome_frag.spv";
    m_pDesc.bindingDescription = &binding;
    m_pDesc.attributeDescriptions = &attribute;
    m_pDesc.attributeCount = 1;
    m_pDesc.depthTest = true;
    m_pDesc.depthWrite = false;
    m_pDesc.blending = false;
    m_pDesc.cullMode = VK_CULL_MODE_FRONT_BIT;
    m_pDesc.frontFace = VK_FRONT_FACE_CLOCKWISE;
    m_pDesc.pushConstants = &push;
    m_pDesc.descriptorLayout = m_descriptorLayout;

    m_pipeline.Create(device, m_pDesc);



}

void SkydomeRenderer::Destroy() {

    m_skyBuffer.Destroy();

    m_descriptorSet = VK_NULL_HANDLE;

    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }

    if (m_descriptorLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorLayout, nullptr);
        m_descriptorLayout = VK_NULL_HANDLE;
    }

    m_pipeline.Destroy(m_device);

}

void SkydomeRenderer::Render(VkCommandBuffer commandBuffer, Skydome& skydome, const Camera& camera) {

    // MAPOWANIE KOLORÓW SKYDOME
    auto* data = static_cast<SkyColors*>(m_skyBuffer.Map());
    *data = skydome.GetSkyColors();
    m_skyBuffer.Unmap();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &m_descriptorSet, 0, nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    // MACIERZE
    fmat4 modelMatrix = ToFloat(lina::Translate(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z));
    fmat4 viewMatrix = ToFloat(camera.GetView());
    fmat4 projectionMatrix = ToFloat(camera.GetProjection());

    SkydomePushConstants push{};
    push.mvp = projectionMatrix * viewMatrix * modelMatrix;

    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SkydomePushConstants), &push);

    // ****************************

    VkBuffer vertexBuffer = skydome.GetVertexBuffer().Get();

    VkDeviceSize offset = 0;

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);

    vkCmdBindIndexBuffer(commandBuffer, skydome.GetIndexBuffer().Get(), 0, VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed(commandBuffer, skydome.GetIndexCount(), 1, 0, 0, 0);

}

void SkydomeRenderer::RecreatePipeline(VkDevice device, VkSampleCountFlagBits samples) {

    m_pipeline.Destroy(device);

    static VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(SkydomeVertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    static VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = sizeof(SkydomePushConstants);

    m_pDesc.samples = samples;
    m_pDesc.bindingDescription = &binding;
    m_pDesc.pushConstants = &push;

    m_pipeline.Create(device, m_pDesc);

}

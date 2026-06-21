#include "SkyquadRenderer.h"

#include "Debug/ErrorDialog.h"
#include "Graphics/Skydome/Skydome.h"
#include "Graphics/Camera/Camera.h"

void SkyquadRenderer::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat colorFormat, VkFormat depthFormat, VkSampleCountFlagBits samples) {

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

    static VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = sizeof(SkyquadPushConstants);

    m_pDesc.colorFormat = colorFormat;
    m_pDesc.depthFormat = depthFormat;
    m_pDesc.samples = samples;
    m_pDesc.vertexShader = "../Engine/Graphics/Skyquad/Shaders/skyquad_vert.spv";
    m_pDesc.fragmentShader = "../Engine/Graphics/Skyquad/Shaders/skyquad_frag.spv";
    m_pDesc.bindingDescription = nullptr;
    m_pDesc.attributeDescriptions = nullptr;
    m_pDesc.attributeCount = 0;
    m_pDesc.depthTest = false;
    m_pDesc.depthWrite = false;
    m_pDesc.blending = false;
    m_pDesc.cullMode = VK_CULL_MODE_BACK_BIT;
    m_pDesc.frontFace = VK_FRONT_FACE_CLOCKWISE;
    m_pDesc.pushConstants = &push;
    m_pDesc.descriptorLayout = m_descriptorLayout;

    m_pipeline.Create(device, m_pDesc);



}

void SkyquadRenderer::Destroy() {

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

void SkyquadRenderer::Render(VkCommandBuffer commandBuffer, Skyquad& skyquad, const Camera& camera) {

    // MAPOWANIE KOLORÓW SKYDOME
    auto* data = static_cast<SkyquadColors*>(m_skyBuffer.Map());
    *data = skyquad.GetSkyColors();
    m_skyBuffer.Unmap();

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &m_descriptorSet, 0, nullptr);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    SkyquadPushConstants push{};
    push.altitude = camera.GetPosition().y;
    push.pitch = camera.GetRotation().x;
    push.roll = camera.GetRotation().z;
    push.fov = static_cast<float>(camera.GetFov());

    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(SkyquadPushConstants), &push);

    vkCmdDraw(commandBuffer, 3, 1, 0 ,0);

}

void SkyquadRenderer::RecreatePipeline(VkDevice device, VkSampleCountFlagBits samples) {

    m_pipeline.Destroy(device);

    static VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(SkydomeVertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    static VkPushConstantRange push{};
    push.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push.offset = 0;
    push.size = sizeof(SkyquadPushConstants);

    m_pDesc.samples = samples;
    m_pDesc.bindingDescription = &binding;
    m_pDesc.pushConstants = &push;

    m_pipeline.Create(device, m_pDesc);

}

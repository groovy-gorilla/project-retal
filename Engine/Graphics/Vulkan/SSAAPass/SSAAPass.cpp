#include "pch.h"
#include "SSAAPass.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"
#include "Core/Settings.h"


void SSAAPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, /*IN*/RenderTarget& sceneColor, Settings& settings) {

    // CREATE RENDER TARGETS
    m_color.Create(device, physicalDevice, renderExtent.width, renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // DESCRIPTOR
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding colorBinding{};
    colorBinding.binding = 0;
    colorBinding.descriptorCount = 1;
    colorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(colorBinding);

    m_descriptor.Create(device, bindings, settings.MAX_FRAMES_IN_FLIGHT);

    for (uint32_t i = 0; i < settings.MAX_FRAMES_IN_FLIGHT; i++) {
        m_descriptor.UpdateTexture(i, 0, sceneColor.GetImageView(), sceneColor.GetLinearSampler());
    }

    PipelineDesc pdesc;
    pdesc.colorFormat = m_color.GetFormat();
    pdesc.depthFormat = VK_FORMAT_UNDEFINED;
    pdesc.descriptorLayout = m_descriptor.GetLayout();
    pdesc.vertexShader = "../Engine/Graphics/Resources/Shaders/SSAA/ssaa_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/SSAA/ssaa_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;

    // PIPELINE
    m_pipeline.Create(device, pdesc);

    std::cout << "[Vulkan] SSAA-render pass created" << std::endl;

}

void SSAAPass::Render(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_color.GetImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width  = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = extent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDescriptorSet descriptorSet = m_descriptor.GetSet(currentFrame);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDraw(commandBuffer, 3, 1,0, 0);

    vkCmdEndRendering(commandBuffer);

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}


void SSAAPass::Destroy(VkDevice device) {

    m_descriptor.Destroy();
    m_color.Destroy(device);
    m_pipeline.Destroy(device);

}

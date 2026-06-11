#include "pch.h"
#include "ScenePass.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"

void ScenePass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, VkFormat depthFormat, AntiAliasing aaMode, VkSampleCountFlagBits samples, Settings& settings) {

    m_renderExtent = renderExtent;
    m_samples = samples;
    m_colorFormat = colorFormat;
    m_depthFormat = depthFormat;

    // CREATE RENDER TARGETS
    m_color.Create(device, physicalDevice, m_renderExtent.width, m_renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, samples);
    m_depth.Create(device, physicalDevice, m_renderExtent.width, m_renderExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, samples);
    m_resolve.Create(device, physicalDevice, m_renderExtent.width, m_renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    std::cout << "[Vulkan] Scene-render pass created" << std::endl;

}

void ScenePass::Destroy(VkDevice device) {

    m_color.Destroy(device);
    m_depth.Destroy(device);
    m_resolve.Destroy(device);

}

void ScenePass::Begin(VkCommandBuffer commandBuffer) {

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_color.GetImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.05f, 0.05f, 0.08f, 1.0f}};

    if (m_samples != VK_SAMPLE_COUNT_1_BIT) {
        colorAttachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachment.resolveImageView = m_resolve.GetImageView();
        colorAttachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkRenderingAttachmentInfo depthAttachment{};
    depthAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthAttachment.imageView = m_depth.GetImageView();
    depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.clearValue.depthStencil = {0.0f, 0};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = m_renderExtent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;
    renderingInfo.pDepthAttachment = &depthAttachment;

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    TransitionImageLayout(commandBuffer,m_depth, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

    if (m_samples != VK_SAMPLE_COUNT_1_BIT) {
        TransitionImageLayout(commandBuffer, m_resolve, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_renderExtent.width);
    viewport.height = static_cast<float>(m_renderExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = m_renderExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

}

void ScenePass::End(VkCommandBuffer commandBuffer) {

    vkCmdEndRendering(commandBuffer);

    if (m_samples == VK_SAMPLE_COUNT_1_BIT) {

        TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    } else {

        TransitionImageLayout(commandBuffer, m_resolve, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    }
}


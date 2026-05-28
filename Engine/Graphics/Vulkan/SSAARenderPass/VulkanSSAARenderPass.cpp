#include "pch.h"
#include "VulkanSSAARenderPass.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"
#include "Core/ApplicationDesc.h"
#include "Debug/ErrorDialog.h"


void VulkanSSAARenderPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, /*IN*/RenderTarget& sceneColor, RenderTarget& sceneDepth, ApplicationDesc& desc) {

    // COLOR
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = colorFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // ATTACHMENT
    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint =VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    // CREATE RENDER PASS
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass));

    // CREATE RENDER TARGETS
    m_color.Create(device, physicalDevice, renderExtent.width, renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // FRAMEBUFFER
    std::vector<VkImageView> attachments = {
        m_color.GetImageView()
    };

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass;
    framebufferCreateInfo.attachmentCount = 1;
    framebufferCreateInfo.pAttachments = attachments.data();
    framebufferCreateInfo.width = renderExtent.width;
    framebufferCreateInfo.height = renderExtent.height;
    framebufferCreateInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_framebuffer));

    // DESCRIPTOR
    m_descriptor.Create(device, desc.MAX_FRAMES_IN_FLIGHT, sceneColor, sceneDepth, TextureFilter::Linear);

    // PIPELINE
    m_pipeline.Create(
        device,
        m_renderPass,
        m_descriptor.GetLayout(),
        nullptr,
        VK_SAMPLE_COUNT_1_BIT,
        "../Engine/Graphics/Resources/Shaders/SSAA/ssaa_vert.spv",
        "../Engine/Graphics/Resources/Shaders/SSAA/ssaa_frag.spv",
        false,
        false);

    std::cout << "[Vulkan] SSAA-render pass created" << std::endl;

}

void VulkanSSAARenderPass::Render(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_renderPass;
    beginInfo.framebuffer = m_framebuffer;
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 1;
    beginInfo.pClearValues = &clear;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

    vkCmdEndRenderPass(commandBuffer);

}


void VulkanSSAARenderPass::Destroy(VkDevice device) {

    m_descriptor.Destroy(device);

    m_color.Destroy(device);

    if (m_framebuffer) {
        vkDestroyFramebuffer(device, m_framebuffer, nullptr);
        m_framebuffer = VK_NULL_HANDLE;
    }

    m_pipeline.Destroy(device);

    if (m_renderPass) {
        vkDestroyRenderPass(device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }

}

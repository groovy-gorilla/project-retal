#include "pch.h"
#include "VulkanSceneRenderPass.h"
#include "Debug/ErrorDialog.h"

void VulkanSceneRenderPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, VkFormat depthFormat, AntiAliasing aaMode, VkSampleCountFlagBits samples, ApplicationDesc& desc) {

  m_renderExtent = renderExtent;

    // COLOR MSAA
    VkAttachmentDescription msaa{};
    msaa.format = colorFormat;
    msaa.samples = samples;
    msaa.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    msaa.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    msaa.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    msaa.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    msaa.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    msaa.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // DEPTH MSAA
    VkAttachmentDescription depth{};
    depth.format = depthFormat;
    depth.samples = samples;
    depth.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout   = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // RESOLVE
    VkAttachmentDescription resolve{};
    resolve.format = colorFormat;
    resolve.samples = VK_SAMPLE_COUNT_1_BIT;
    resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    resolve.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // ATTACHMENTS
    std::vector<VkAttachmentDescription> attachments;
    attachments.push_back(msaa);            // 0
    attachments.push_back(depth);           // 1
    attachments.push_back(resolve);         // 2

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef{};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference resolveRef{};
    resolveRef.attachment = 2;
    resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;

    if (aaMode == AntiAliasing::MSAA || aaMode == AntiAliasing::MSAA_SMAA) {
        subpass.pResolveAttachments = &resolveRef;
    } else {
        subpass.pResolveAttachments = nullptr;
    }

    // DEPENDENCIES
    VkSubpassDependency dep1{};
    dep1.srcSubpass = VK_SUBPASS_EXTERNAL;
    dep1.dstSubpass = 0;
    dep1.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dep1.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dep1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dep2{};
    dep2.srcSubpass = 0;
    dep2.dstSubpass = VK_SUBPASS_EXTERNAL;
    dep2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dep2.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dep2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkSubpassDependency dependencies[] = { dep1, dep2 };

    // CREATE RENDER PASS
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    createInfo.pAttachments = attachments.data();
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 2;
    createInfo.pDependencies = dependencies;

    VK_CHECK(vkCreateRenderPass(device, &createInfo, nullptr, &m_renderPass));

    // CREATE RENDER TARGETS
    m_color.Create(device, physicalDevice, m_renderExtent.width, m_renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, samples);
    m_depth.Create(device, physicalDevice, m_renderExtent.width, m_renderExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, samples);
    m_resolve.Create(device, physicalDevice, m_renderExtent.width, m_renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // FRAMEBUFFER
    std::vector<VkImageView> imageAttachments = {
        m_color.GetImageView(),
        m_depth.GetImageView(),
        m_resolve.GetImageView()
    };

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass;
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageAttachments.size());
    framebufferCreateInfo.pAttachments = imageAttachments.data();
    framebufferCreateInfo.width = m_renderExtent.width;
    framebufferCreateInfo.height = m_renderExtent.height;
    framebufferCreateInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_framebuffer));

    // PIPELINE
    m_pipeline.Create(
        device,
        m_renderPass,
        nullptr,
        nullptr,
        samples,
        "../Engine/Graphics/Resources/Shaders/Scene/scene_vert.spv",
        "../Engine/Graphics/Resources/Shaders/Scene/scene_frag.spv",
        true,
        false);

    std::cout << "[Vulkan] Scene-render pass created" << std::endl;

}

void VulkanSceneRenderPass::Destroy(VkDevice device) {

    m_pipeline.Destroy(device);

    if (m_framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(device, m_framebuffer, nullptr);
        m_framebuffer = VK_NULL_HANDLE;
    }

    m_color.Destroy(device);
    m_depth.Destroy(device);
    m_resolve.Destroy(device);

    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
        std::cout << "[Vulkan] Scene-render pass destroyed" << std::endl;
    }

}

void VulkanSceneRenderPass::Begin(VkCommandBuffer commandBuffer) {

    VkClearValue clearValues[3]{};

    // COLOR
    clearValues[0].color = {0.05f, 0.05f, 0.08f, 1.0f};

    // DEPTH
    clearValues[1].depthStencil = { 1.0f, 0 };

    // RESOLVE COLOR
    clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_renderPass;
    beginInfo.framebuffer = m_framebuffer;
    beginInfo.renderArea.extent = m_renderExtent;
    beginInfo.clearValueCount = 3;
    beginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

}

void VulkanSceneRenderPass::End(VkCommandBuffer commandBuffer) {

    vkCmdEndRenderPass(commandBuffer);

}

void VulkanSceneRenderPass::PipelineBind(VkCommandBuffer commandBuffer) {

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

}

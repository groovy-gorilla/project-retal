#include "pch.h"
#include "VulkanPostRenderPass.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Debug/ErrorDialog.h"
#include "Core/ApplicationDesc.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

void VulkanPostRenderPass::Create(VkDevice device, VkExtent2D extent, VkFormat swapchainFormat, ApplicationDesc desc) {

    // COLOR
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // ATTACHMENT
    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    // CREATE RENDER PASS
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;

    VK_CHECK(vkCreateRenderPass(device, &createInfo, nullptr, &m_renderPass));

    // DESCRIPTOR
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding colorBinding{};
    colorBinding.binding = 0;
    colorBinding.descriptorCount = 1;
    colorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(colorBinding);

    VkDescriptorSetLayoutBinding depthBinding{};
    depthBinding.binding = 1;
    depthBinding.descriptorCount = 1;
    depthBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    depthBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(depthBinding);

    m_descriptor.Create(device, bindings, desc.MAX_FRAMES_IN_FLIGHT);

    // PUSH CONSTANT
    VkPushConstantRange pushConstant{};
    pushConstant.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstant.offset = 0;
    pushConstant.size = sizeof(PostPushConstants);

    // PIPELINE
    m_pipeline.Create(
        device,
        m_renderPass,
        m_descriptor.GetLayout(),
        &pushConstant,
        VK_SAMPLE_COUNT_1_BIT,
        "../Engine/Graphics/Resources/Shaders/Post/post_vert.spv",
        "../Engine/Graphics/Resources/Shaders/Post/post_frag.spv",
        false,
        false);

    std::cout << "[Vulkan] Post-render pass created" << std::endl;

}

void VulkanPostRenderPass::Render(VkDevice device, uint32_t frameIndex, VkCommandBuffer commandBuffer, RenderTarget& inputColor, VkFramebuffer framebuffer, VkExtent2D extent, ApplicationDesc& desc, float exposure) {

    VkSampler sampler = desc.FILTER == TextureFilter::Nearest ? inputColor.GetNearestSampler() : inputColor.GetLinearSampler();
    m_descriptor.UpdateTexture(frameIndex, 0, inputColor.GetImageView(), sampler);

    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 1;
    beginInfo.pClearValues = &clear;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};

    if (desc.ASPECT_RATIO) {

        float targetAspect = static_cast<float>(desc.WIDTH) / static_cast<float>(desc.HEIGHT);
        float windowAspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);
        float viewportWidth = static_cast<float>(extent.width);
        float viewportHeight = static_cast<float>(extent.height);
        float viewportX = 0.0f;
        float viewportY = 0.0f;

        if (windowAspect > targetAspect) {
            viewportWidth = viewportHeight * targetAspect;
            viewportX = (extent.width - viewportWidth) * 0.5f;
        } else {
            viewportHeight = viewportWidth / targetAspect;
            viewportY = (extent.height - viewportHeight) * 0.5f;
        }

        viewport.x = viewportX;
        viewport.y = viewportY;
        viewport.width = viewportWidth;
        viewport.height = viewportHeight;

        scissor.offset = {
            static_cast<int32_t>(viewportX),
            static_cast<int32_t>(viewportY)
        };

        scissor.extent = {
            static_cast<uint32_t>(viewportWidth),
            static_cast<uint32_t>(viewportHeight)
        };
    } else {
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);

        scissor.offset = { 0, 0 };
        scissor.extent = extent;
    }

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    PostPushConstants PC;
    PC.hdrEnable = desc.HDR;
    PC.exposure = exposure;
    PC.dithering = desc.DITHERING;

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());
    vkCmdPushConstants(commandBuffer, m_pipeline.GetLayout(), VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PostPushConstants), &PC);


    VkDescriptorSet descriptorSet = m_descriptor.GetSet(frameIndex);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDraw(commandBuffer, 3, 1,0, 0);

    vkCmdEndRenderPass(commandBuffer);

}

void VulkanPostRenderPass::Destroy(VkDevice device) {

    m_descriptor.Destroy();
    m_pipeline.Destroy(device);

    if (m_renderPass) {
        vkDestroyRenderPass(device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }

}


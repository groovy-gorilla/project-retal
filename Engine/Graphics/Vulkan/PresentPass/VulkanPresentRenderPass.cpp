#include "pch.h"
#include "VulkanPresentRenderPass.h"
#include "Debug/ErrorDialog.h"
#include "Core/Settings.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

void VulkanPresentRenderPass::Create(VkDevice device, VkFormat swapchainFormat, Settings& settings) {

    m_device = device;

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

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // CREATE RENDER PASS
    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(device, &createInfo, nullptr, &m_renderPass));

    // DESCRIPTOR
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding colorBinding{};
    colorBinding.binding = 0;
    colorBinding.descriptorCount = 1;
    colorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(colorBinding);

    m_descriptor.Create(device, bindings, settings.MAX_FRAMES_IN_FLIGHT);

    PipelineDesc pdesc;
    pdesc.renderPass = m_renderPass;
    pdesc.descriptorLayout = m_descriptor.GetLayout();
    pdesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Present/present_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Present/present_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;

    // PIPELINE
    m_pipeline.Create(device, pdesc);

    std::cout << "[Vulkan] Overlay-render pass created" << std::endl;

}

void VulkanPresentRenderPass::Render(uint32_t frameIndex, VkCommandBuffer commandBuffer, RenderTarget& inputColor, VkFramebuffer framebuffer, VkExtent2D extent, Settings& settings) {

    // UPDATE DESCRIPTOR
    VkSampler sampler = settings.FILTER == TextureFilter::Nearest ? inputColor.GetNearestSampler() : inputColor.GetLinearSampler();
    m_descriptor.UpdateTexture(frameIndex, 0, inputColor.GetImageView(), sampler);

    // CLEAR
    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_renderPass;
    beginInfo.framebuffer = framebuffer;
    beginInfo.renderArea.offset = {0,0 };
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 1;
    beginInfo.pClearValues = &clear;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};

    if (settings.ASPECT_RATIO) {

        float targetAspect = static_cast<float>(settings.WIDTH) / static_cast<float>(settings.HEIGHT);
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


    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    VkDescriptorSet descriptorSet = m_descriptor.GetSet(frameIndex);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1,0, 0);

    vkCmdEndRenderPass(commandBuffer);

}

void VulkanPresentRenderPass::Destroy() {

    m_descriptor.Destroy();
    m_pipeline.Destroy(m_device);

    if (m_renderPass) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        m_renderPass = VK_NULL_HANDLE;
    }

}


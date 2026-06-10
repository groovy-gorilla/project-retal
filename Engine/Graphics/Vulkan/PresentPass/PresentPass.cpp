#include "pch.h"
#include "PresentPass.h"
#include "Core/Settings.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

void PresentPass::Create(VkDevice device, VkFormat swapchainFormat, Settings& settings) {

    m_device = device;

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
    pdesc.renderPass = VK_NULL_HANDLE;
    pdesc.colorFormat = swapchainFormat;
    pdesc.depthFormat = VK_FORMAT_UNDEFINED;
    pdesc.descriptorLayout = m_descriptor.GetLayout();
    pdesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Present/present_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Present/present_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;

    // PIPELINE
    m_pipeline.Create(device, pdesc);

    std::cout << "[Vulkan] Present-render pass created" << std::endl;

}

void PresentPass::Render(uint32_t frameIndex, VkCommandBuffer commandBuffer, RenderTarget& inputColor, VkImage swapchainImage, VkImageView swapchainView, VkExtent2D extent, Settings& settings) {

    // UPDATE DESCRIPTOR
    VkSampler sampler = settings.FILTER == TextureFilter::Nearest ? inputColor.GetNearestSampler() : inputColor.GetLinearSampler();
    m_descriptor.UpdateTexture(frameIndex, 0, inputColor.GetImageView(), sampler);

    // CLEAR
    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = swapchainView;
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0,0};
    renderingInfo.renderArea.extent = extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    TransitionImageLayout2(commandBuffer, swapchainImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

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

    vkCmdEndRendering(commandBuffer);

    TransitionImageLayout2(commandBuffer, swapchainImage, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

}

void PresentPass::Destroy() {

    m_descriptor.Destroy();
    m_pipeline.Destroy(m_device);

}


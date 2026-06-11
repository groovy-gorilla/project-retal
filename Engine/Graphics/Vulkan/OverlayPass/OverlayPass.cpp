#include "pch.h"
#include "OverlayPass.h"
#include "Core/Settings.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "Graphics/Vulkan/Wrappers/RenderTarget.h"

void OverlayPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, Settings& settings) {

    m_device = device;
    m_colorFormat = colorFormat;

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

    PipelineDesc pdesc;
    pdesc.colorFormat = colorFormat;
    pdesc.depthFormat = VK_FORMAT_UNDEFINED;
    pdesc.descriptorLayout = m_descriptor.GetLayout();
    pdesc.vertexShader = "../Engine/Graphics/Resources/Shaders/Overlay/overlay_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Resources/Shaders/Overlay/overlay_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;

    // PIPELINE
    m_pipeline.Create(device, pdesc);

    std::cout << "[Vulkan] Overlay-render pass created" << std::endl;

}

void OverlayPass::Begin(uint32_t frameIndex, VkCommandBuffer commandBuffer, RenderTarget& inputColor, VkExtent2D extent, Settings& settings) {

    // UPDATE DESCRIPTOR
    VkSampler sampler = settings.FILTER == TextureFilter::Nearest ? inputColor.GetNearestSampler() : inputColor.GetLinearSampler();
    m_descriptor.UpdateTexture(frameIndex, 0, inputColor.GetImageView(), sampler);

    // CLEAR
    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 1.0f };

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_color.GetImageView();
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

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0,1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.Get());

    VkDescriptorSet descriptorSet = m_descriptor.GetSet(frameIndex);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1,0, 0);

}

void OverlayPass::End(VkCommandBuffer commandBuffer) {

    vkCmdEndRendering(commandBuffer);

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void OverlayPass::Destroy(VkDevice device) {

    m_descriptor.Destroy();
    m_color.Destroy(device);
    m_pipeline.Destroy(m_device);

}


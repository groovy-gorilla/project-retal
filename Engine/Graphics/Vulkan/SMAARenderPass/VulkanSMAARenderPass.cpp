#include "pch.h"
#include "VulkanSMAARenderPass.h"

#include "Core/ApplicationDesc.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "ThirdParty/smaa_textures/AreaTex.h"
#include "ThirdParty/smaa_textures/SearchTex.h"
#include "Debug/ErrorDialog.h"

void VulkanSMAARenderPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, ApplicationDesc& desc, VkCommandPool commandPool, VkQueue graphicsQueue) {

    m_device = device;
    m_commandPool = commandPool;
    m_graphicsQueue = graphicsQueue;

    // AREA TEXTURE
    m_areaTexture.Create(device, physicalDevice, 160, 560, VK_FORMAT_R8G8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // AREA STAGING BUFFER
    VkBuffer areaStagingBuffer{};
    VkDeviceMemory areaStagingMemory{};
    VkDeviceSize areaSize = sizeof(areaTexBytes);
    CreateBuffer(device, physicalDevice, areaSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, areaStagingBuffer, areaStagingMemory);

    // COPY AREA DATA
    void* data{};
    vkMapMemory(device, areaStagingMemory, 0, areaSize, 0, &data);
    memcpy(data, areaTexBytes, static_cast<size_t>(areaSize));
    vkUnmapMemory(device, areaStagingMemory);
    TransitionImageLayout(device, m_commandPool, m_graphicsQueue, m_areaTexture.GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(device, m_commandPool, m_graphicsQueue, areaStagingBuffer, m_areaTexture.GetImage(), m_areaTexture.GetWidth(), m_areaTexture.GetHeight());
    TransitionImageLayout(device, m_commandPool, m_graphicsQueue, m_areaTexture.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkDestroyBuffer(device, areaStagingBuffer, nullptr);
    vkFreeMemory(device, areaStagingMemory, nullptr);

    // SEARCH TEXTURE
    m_searchTexture.Create(device, physicalDevice, 64, 16, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // SEARCH STAGING BUFFER
    VkBuffer searchStagingBuffer{};
    VkDeviceMemory searchStagingMemory{};
    VkDeviceSize searchSize = sizeof(searchTexBytes);
    CreateBuffer(device, physicalDevice, searchSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, searchStagingBuffer, searchStagingMemory);

    // COPY SEARCH DATA
    vkMapMemory(device, searchStagingMemory, 0, searchSize, 0, &data);
    memcpy(data, searchTexBytes, static_cast<size_t>(searchSize));
    vkUnmapMemory(device, searchStagingMemory);
    TransitionImageLayout(device, m_commandPool, m_graphicsQueue, m_searchTexture.GetImage(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(device, m_commandPool, m_graphicsQueue, searchStagingBuffer, m_searchTexture.GetImage(), m_searchTexture.GetWidth(), m_searchTexture.GetHeight());
    TransitionImageLayout(device, m_commandPool, m_graphicsQueue, m_searchTexture.GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    vkDestroyBuffer(device, searchStagingBuffer, nullptr);
    vkFreeMemory(device, searchStagingMemory, nullptr);

    // EDGE TARGET
    m_edgeColor.Create(device, physicalDevice, renderExtent.width, renderExtent.height, VK_FORMAT_R8G8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // BLEND TARGET
    m_blendColor.Create(device, physicalDevice, renderExtent.width, renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // OUTPUT TARGET
    m_color.Create(device, physicalDevice, renderExtent.width, renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);


    // EDGE PASS
    CreateEdgeRenderPass(device);
    CreateEdgeFramebuffer(device, renderExtent);
    CreateEdgeDescriptors(device, desc);
    CreateEdgePipeline(device, renderExtent);

    // BLEND PASS
    CreateBlendRenderPass(device);
    CreateBlendFramebuffer(device, renderExtent);
    CreateBlendDescriptors(device, desc);
    CreateBlendPipeline(device, renderExtent);

    // NEIGHBORHOOD PASS
    CreateNeighborhoodRenderPass(device, m_color);
    CreateNeighborhoodFramebuffer(device, renderExtent, m_color);
    CreateNeighborhoodDescriptors(device, desc);
    CreateNeighborhoodPipeline(device, renderExtent);

    std::cout << "[Vulkan] SMAA-render pass created" << std::endl;

}

void VulkanSMAARenderPass::Render(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame, RenderTarget& inputColor) {

    // EDGE
    m_edgeDescriptor.UpdateColor(m_device, currentFrame, inputColor, TextureFilter::Linear);
    RenderEdgePass(commandBuffer, extent, currentFrame);

    // BLEND
    m_blendDescriptor.UpdateSMAABlend(m_device, currentFrame, m_edgeColor, m_areaTexture, m_searchTexture);
    RenderBlendPass(commandBuffer, extent, currentFrame);

    // NEIGHBORHOOD
    m_neighborhoodDescriptor.UpdateSMAANeighborhood(m_device, currentFrame, inputColor, m_blendColor);
    RenderNeighborhoodPass(commandBuffer, extent, currentFrame);

}

void VulkanSMAARenderPass::Destroy(VkDevice device) {

    // DESCRIPTORS
    m_edgeDescriptor.Destroy(device);
    m_blendDescriptor.Destroy(device);
    m_neighborhoodDescriptor.Destroy(device);
    m_color.Destroy(device);

    // EDGE PASS
    m_edgePipeline.Destroy(device);

    if (m_edgeFramebuffer) {
        vkDestroyFramebuffer(device, m_edgeFramebuffer, nullptr);
        m_edgeFramebuffer = VK_NULL_HANDLE;
    }

    if (m_edgeRenderPass) {
        vkDestroyRenderPass(device, m_edgeRenderPass, nullptr);
        m_edgeRenderPass = VK_NULL_HANDLE;
    }

    // BLEND PASS
    m_blendPipeline.Destroy(device);

    if (m_blendFramebuffer) {
        vkDestroyFramebuffer(device, m_blendFramebuffer, nullptr);
        m_blendFramebuffer = VK_NULL_HANDLE;
    }

    if (m_blendRenderPass) {
        vkDestroyRenderPass(device, m_blendRenderPass, nullptr);
        m_blendRenderPass = VK_NULL_HANDLE;
    }

    // NEIGHBORHOOD PASS
    m_neighborhoodPipeline.Destroy(device);

    if (m_neighborhoodFramebuffer) {
        vkDestroyFramebuffer(device, m_neighborhoodFramebuffer, nullptr);
        m_neighborhoodFramebuffer = VK_NULL_HANDLE;
    }

    if (m_neighborhoodRenderPass) {
        vkDestroyRenderPass(device, m_neighborhoodRenderPass, nullptr);
        m_neighborhoodRenderPass = VK_NULL_HANDLE;
    }

    // TARGETS
    m_edgeColor.Destroy(device);
    m_blendColor.Destroy(device);

    // LOOKUP TEXTURES
    m_areaTexture.Destroy(device);
    m_searchTexture.Destroy(device);

    std::cout << "[Vulkan] SMAA-render pass destroyed" << std::endl;

}

void VulkanSMAARenderPass::RenderEdgePass(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    // CLEAR
    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 0.0f };

    // RENDER PASS BEGIN
    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_edgeRenderPass;
    beginInfo.framebuffer = m_edgeFramebuffer;
    beginInfo.renderArea.offset = { 0, 0 };
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 1;
    beginInfo.pClearValues = &clear;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // PIPELINE
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_edgePipeline.Get());

    // DESCRIPTORS
    VkDescriptorSet descriptorSet = m_edgeDescriptor.GetSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_edgePipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRenderPass(commandBuffer);

}

void VulkanSMAARenderPass::RenderBlendPass(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    // CLEAR
    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 0.0f };

    // RENDER PASS BEGIN
    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_blendRenderPass;
    beginInfo.framebuffer = m_blendFramebuffer;
    beginInfo.renderArea.offset = { 0, 0 };
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 1;
    beginInfo.pClearValues = &clear;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // PIPELINE
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, m_blendPipeline.Get());

    // DESCRIPTORS
    VkDescriptorSet descriptorSet = m_blendDescriptor.GetSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blendPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRenderPass(commandBuffer);

}

void VulkanSMAARenderPass::RenderNeighborhoodPass(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    // CLEAR
    VkClearValue clear{};
    clear.color = { 0.0f, 0.0f, 0.0f, 1.0f };

    // RENDER PASS BEGIN
    VkRenderPassBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderPass = m_neighborhoodRenderPass;
    beginInfo.framebuffer = m_neighborhoodFramebuffer;
    beginInfo.renderArea.offset = { 0, 0 };
    beginInfo.renderArea.extent = extent;
    beginInfo.clearValueCount = 1;
    beginInfo.pClearValues = &clear;

    vkCmdBeginRenderPass(commandBuffer, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // PIPELINE
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_neighborhoodPipeline.Get());

    // DESCRIPTORS
    VkDescriptorSet descriptorSet = m_neighborhoodDescriptor.GetSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_neighborhoodPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRenderPass(commandBuffer);

}

void VulkanSMAARenderPass::CreateEdgeRenderPass(VkDevice device) {

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_edgeColor.GetFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // ATTACHMENT REFERENCE
    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    // DEPENDENCY
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dependency2{};
    dependency2.srcSubpass = 0;
    dependency2.dstSubpass = VK_SUBPASS_EXTERNAL;
    dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency2.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    VkSubpassDependency dependencies[] = {
        dependency,
        dependency2
    };

    // RENDER PASS
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 2;
    renderPassInfo.pDependencies = dependencies;

    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_edgeRenderPass));

}

void VulkanSMAARenderPass::CreateEdgeFramebuffer(VkDevice device, VkExtent2D extent) {

    // ATTACHMENTS
    VkImageView attachments[] = {
        m_edgeColor.GetImageView()
    };

    // FRAMEBUFFER INFO
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_edgeRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    // CREATE
    VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_edgeFramebuffer));

}

void VulkanSMAARenderPass::CreateEdgeDescriptors(VkDevice device, ApplicationDesc& desc) {

    // INPUT COLOR
    m_edgeDescriptor.CreateColor(device, desc.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_edgeDescriptorLayout = m_edgeDescriptor.GetLayout();

}

void VulkanSMAARenderPass::CreateEdgePipeline(VkDevice device, VkExtent2D extent) {

    struct SMAASpecializationData {
        float width;
        float height;
        float inverseWidth;
        float inverseHeight;
    };

    SMAASpecializationData specData{};
    specData.width = static_cast<float>(extent.width);
    specData.height = static_cast<float>(extent.height);
    specData.inverseWidth = 1.0f / specData.width;
    specData.inverseHeight = 1.0f / specData.height;

    VkSpecializationMapEntry entries[4]{};

    entries[0] = {
        0,
        offsetof(SMAASpecializationData, width),
        sizeof(float)
    };

    entries[1] = {
        1,
        offsetof(SMAASpecializationData, height),
        sizeof(float)
    };

    entries[2] = {
        2,
        offsetof(SMAASpecializationData, inverseWidth),
        sizeof(float)
    };

    entries[3] = {
        3,
        offsetof(SMAASpecializationData, inverseHeight),
        sizeof(float)
    };

    VkSpecializationInfo specializationInfo{};
    specializationInfo.mapEntryCount = 4;
    specializationInfo.pMapEntries = entries;
    specializationInfo.dataSize = sizeof(specData);
    specializationInfo.pData = &specData;

    m_edgePipeline.Create(
        device,
        m_edgeRenderPass,
        m_edgeDescriptorLayout,
        nullptr,
        VK_SAMPLE_COUNT_1_BIT,
        "../Engine/Graphics/Resources/Shaders/SMAA/smaa_edge_vert.spv",
        "../Engine/Graphics/Resources/Shaders/SMAA/smaa_edge_frag.spv",
        false,
        false,
        &specializationInfo,
        &specializationInfo);

}

void VulkanSMAARenderPass::CreateBlendRenderPass(VkDevice device) {

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_blendColor.GetFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // ATTACHMENT REF
    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    // DEPENDENCY
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // RENDER PASS
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_blendRenderPass));

}

void VulkanSMAARenderPass::CreateBlendFramebuffer(VkDevice device, VkExtent2D extent) {

    // ATTACHMENTS
    VkImageView attachments[] = {
        m_blendColor.GetImageView()
    };

    // FRAMEBUFFER INFO
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_blendRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    // CREATE
    VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_blendFramebuffer));

}

void VulkanSMAARenderPass::CreateBlendDescriptors(VkDevice device, ApplicationDesc& desc) {

    // EDGE TEXTURE
    m_blendDescriptor.CreateSMAABlend(device, desc.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_blendDescriptorLayout = m_blendDescriptor.GetLayout();

}

void VulkanSMAARenderPass::CreateBlendPipeline(VkDevice device, VkExtent2D extent) {

    struct SMAASpecializationData {
        float width;
        float height;
        float inverseWidth;
        float inverseHeight;
    };

    SMAASpecializationData specData{};
    specData.width = static_cast<float>(extent.width);
    specData.height = static_cast<float>(extent.height);
    specData.inverseWidth = 1.0f / specData.width;
    specData.inverseHeight = 1.0f / specData.height;

    VkSpecializationMapEntry entries[4]{};

    entries[0] = {
        0,
        offsetof(SMAASpecializationData, width),
        sizeof(float)
    };

    entries[1] = {
        1,
        offsetof(SMAASpecializationData, height),
        sizeof(float)
    };

    entries[2] = {
        2,
        offsetof(SMAASpecializationData, inverseWidth),
        sizeof(float)
    };

    entries[3] = {
        3,
        offsetof(SMAASpecializationData, inverseHeight),
        sizeof(float)
    };

    VkSpecializationInfo specializationInfo{};
    specializationInfo.mapEntryCount = 4;
    specializationInfo.pMapEntries = entries;
    specializationInfo.dataSize = sizeof(specData);
    specializationInfo.pData = &specData;

    m_blendPipeline.Create(
        device,
        m_blendRenderPass,
        m_blendDescriptorLayout,
        nullptr,
        VK_SAMPLE_COUNT_1_BIT,
        "../Engine/Graphics/Resources/Shaders/SMAA/smaa_blend_vert.spv",
        "../Engine/Graphics/Resources/Shaders/SMAA/smaa_blend_frag.spv",
        false,
        false,
        &specializationInfo,
        &specializationInfo);

}

void VulkanSMAARenderPass::CreateNeighborhoodRenderPass(VkDevice device, RenderTarget& outputColor) {

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = outputColor.GetFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    // ATTACHMENT REF
    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    // DEPENDENCY
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // RENDER PASS
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_neighborhoodRenderPass));

}

void VulkanSMAARenderPass::CreateNeighborhoodFramebuffer(VkDevice device, VkExtent2D extent, RenderTarget& outputColor) {

    // ATTACHMENTS
    VkImageView attachments[] = {
        outputColor.GetImageView()
    };

    // FRAMEBUFFER INFO
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = m_neighborhoodRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    // CREATE
    VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_neighborhoodFramebuffer));

}

void VulkanSMAARenderPass::CreateNeighborhoodDescriptors(VkDevice device, ApplicationDesc& desc) {

    // BLEND TEXTURE
    m_neighborhoodDescriptor.CreateSMAANeighborhood(device, desc.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_neighborhoodDescriptorLayout = m_neighborhoodDescriptor.GetLayout();

}

void VulkanSMAARenderPass::CreateNeighborhoodPipeline(VkDevice device, VkExtent2D extent) {

    struct SMAASpecializationData {
        float width;
        float height;
        float inverseWidth;
        float inverseHeight;
    };

    SMAASpecializationData specData{};
    specData.width = static_cast<float>(extent.width);
    specData.height = static_cast<float>(extent.height);
    specData.inverseWidth = 1.0f / specData.width;
    specData.inverseHeight = 1.0f / specData.height;

    VkSpecializationMapEntry entries[4]{};

    entries[0] = {
        0,
        offsetof(SMAASpecializationData, width),
        sizeof(float)
    };

    entries[1] = {
        1,
        offsetof(SMAASpecializationData, height),
        sizeof(float)
    };

    entries[2] = {
        2,
        offsetof(SMAASpecializationData, inverseWidth),
        sizeof(float)
    };

    entries[3] = {
        3,
        offsetof(SMAASpecializationData, inverseHeight),
        sizeof(float)
    };

    VkSpecializationInfo specializationInfo{};
    specializationInfo.mapEntryCount = 4;
    specializationInfo.pMapEntries = entries;
    specializationInfo.dataSize = sizeof(specData);
    specializationInfo.pData = &specData;

    m_neighborhoodPipeline.Create(
        device,
        m_neighborhoodRenderPass,
        m_neighborhoodDescriptorLayout,
        nullptr,
        VK_SAMPLE_COUNT_1_BIT,
        "../Engine/Graphics/Resources/Shaders/SMAA/smaa_neighborhood_vert.spv",
        "../Engine/Graphics/Resources/Shaders/SMAA/smaa_neighborhood_frag.spv",
        false,
        false,
        &specializationInfo,
        &specializationInfo);

}
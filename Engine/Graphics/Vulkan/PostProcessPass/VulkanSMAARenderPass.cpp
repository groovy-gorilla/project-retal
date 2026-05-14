#include "pch.h"
#include "VulkanSMAARenderPass.h"

#include "Core/ApplicationDesc.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"

void VulkanSMAARenderPass::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, RenderTarget& outputColor, ApplicationDesc& desc) {

    m_device = device;

    // OUTPUT
    m_outputColor = &outputColor;

    // EDGE TARGET
    m_edgeTarget.Width = extent.width;
    m_edgeTarget.Height = extent.height;
    m_edgeTarget.Format = VK_FORMAT_R8G8_UNORM;
    CreateImage(physicalDevice, device, extent.width, extent.height, m_edgeTarget.Format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, m_edgeTarget.Image, m_edgeTarget.Memory);
    m_edgeTarget.View = CreateImageView(device, m_edgeTarget.Image, m_edgeTarget.Format, VK_IMAGE_ASPECT_COLOR_BIT);
    m_edgeTarget.CreateSamplers(device);

    // BLEND TARGET
    m_blendTarget.Width = extent.width;
    m_blendTarget.Height = extent.height;
    m_blendTarget.Format = VK_FORMAT_R8G8B8A8_UNORM;
    CreateImage(physicalDevice, device, extent.width, extent.height, m_blendTarget.Format, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_SAMPLE_COUNT_1_BIT, m_blendTarget.Image, m_blendTarget.Memory);
    m_blendTarget.View = CreateImageView(device, m_blendTarget.Image, m_blendTarget.Format, VK_IMAGE_ASPECT_COLOR_BIT);
    m_blendTarget.CreateSamplers(device);

    // EDGE PASS
    CreateEdgeRenderPass(device);
    CreateEdgeFramebuffer(device, extent);
    CreateEdgeDescriptors(device, desc);
    CreateEdgePipeline(device, extent);

    // BLEND PASS
    CreateBlendRenderPass(device);
    CreateBlendFramebuffer(device, extent);
    CreateBlendDescriptors(device, desc);
    CreateBlendPipeline(device, extent);

    // NEIGHBORHOOD PASS
    CreateNeighborhoodRenderPass(device);
    CreateNeighborhoodFramebuffer(device, extent);
    CreateNeighborhoodDescriptors(device, desc);
    CreateNeighborhoodPipeline(device, extent);

    std::cout << "[Vulkan] SMAA render pass created" << std::endl;

}

void VulkanSMAARenderPass::Render(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame, RenderTarget& inputColor) {

    // EDGE
    //m_edgeDescriptor.UpdateColor(m_device, currentFrame, inputColor, TextureFilter::Linear);
    RenderEdgePass(commandBuffer, extent, currentFrame);

    // BLEND
    //m_blendDescriptor.UpdateColor(m_device, currentFrame, m_edgeTarget, TextureFilter::Linear);
    RenderBlendPass(commandBuffer, extent, currentFrame);

    // NEIGHBORHOOD
    //m_neighborhoodDescriptor.UpdateColor(m_device, currentFrame, m_blendTarget, TextureFilter::Linear);
    RenderNeighborhoodPass(commandBuffer, extent, currentFrame);

}

void VulkanSMAARenderPass::Destroy(VkDevice device) {

    // DESCRIPTORS
    m_edgeDescriptor.Destroy(device);
    m_blendDescriptor.Destroy(device);
    m_neighborhoodDescriptor.Destroy(device);

    // EDGE PASS
    if (m_edgePipeline) {
        vkDestroyPipeline(device, m_edgePipeline, nullptr);
        m_edgePipeline = VK_NULL_HANDLE;
    }

    if (m_edgePipelineLayout) {
        vkDestroyPipelineLayout(device, m_edgePipelineLayout, nullptr);
        m_edgePipelineLayout = VK_NULL_HANDLE;
    }

    if (m_edgeFramebuffer) {
        vkDestroyFramebuffer(device, m_edgeFramebuffer, nullptr);
        m_edgeFramebuffer = VK_NULL_HANDLE;
    }

    if (m_edgeRenderPass) {
        vkDestroyRenderPass(device, m_edgeRenderPass, nullptr);
        m_edgeRenderPass = VK_NULL_HANDLE;
    }

    // BLEND PASS
    if (m_blendPipeline) {
        vkDestroyPipeline(device, m_blendPipeline, nullptr);
        m_blendPipeline = VK_NULL_HANDLE;
    }

    if (m_blendPipelineLayout) {
        vkDestroyPipelineLayout(device, m_blendPipelineLayout, nullptr);
        m_blendPipelineLayout = VK_NULL_HANDLE;
    }

    if (m_blendFramebuffer) {
        vkDestroyFramebuffer(device, m_blendFramebuffer, nullptr);
        m_blendFramebuffer = VK_NULL_HANDLE;
    }

    if (m_blendRenderPass) {
        vkDestroyRenderPass(device, m_blendRenderPass, nullptr);
        m_blendRenderPass = VK_NULL_HANDLE;
    }

    // NEIGHBORHOOD PASS
    if (m_neighborhoodPipeline) {
        vkDestroyPipeline(device, m_neighborhoodPipeline, nullptr);
        m_neighborhoodPipeline = VK_NULL_HANDLE;
    }

    if (m_neighborhoodPipelineLayout) {
        vkDestroyPipelineLayout(device, m_neighborhoodPipelineLayout, nullptr);
        m_neighborhoodPipelineLayout = VK_NULL_HANDLE;
    }

    if (m_neighborhoodFramebuffer) {
        vkDestroyFramebuffer(device, m_neighborhoodFramebuffer, nullptr);
        m_neighborhoodFramebuffer = VK_NULL_HANDLE;
    }

    if (m_neighborhoodRenderPass) {
        vkDestroyRenderPass(device, m_neighborhoodRenderPass, nullptr);
        m_neighborhoodRenderPass = VK_NULL_HANDLE;
    }

    // TARGETS
    m_edgeTarget.Destroy(device);
    m_blendTarget.Destroy(device);

    // LOOKUP TEXTURES
    m_areaTexture.Destroy(device);
    m_searchTexture.Destroy(device);

    std::cout << "[Vulkan] SMAA render pass destroyed" << std::endl;

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
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_edgePipeline);

    // DESCRIPTORS
    //VkDescriptorSet descriptorSet = m_edgeDescriptor.GetSet(currentFrame);
    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_edgePipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

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
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, m_blendPipeline);

    // DESCRIPTORS
    //VkDescriptorSet descriptorSet = m_blendDescriptor.GetSet(currentFrame);
    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blendPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

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
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_neighborhoodPipeline);

    // DESCRIPTORS
    //VkDescriptorSet descriptorSet = m_neighborhoodDescriptor.GetSet(currentFrame);
    //vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_neighborhoodPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRenderPass(commandBuffer);

}

void VulkanSMAARenderPass::CreateEdgeRenderPass(VkDevice device) {

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_edgeTarget.Format;
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

    VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_edgeRenderPass));

}

void VulkanSMAARenderPass::CreateEdgeFramebuffer(VkDevice device, VkExtent2D extent) {

    // ATTACHMENTS
    VkImageView attachments[] = {
        m_edgeTarget.View
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
    //m_edgeDescriptor.CreateColor(device, desc.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_edgeDescriptorLayout = m_edgeDescriptor.GetLayout();

}

void VulkanSMAARenderPass::CreateEdgePipeline(VkDevice device, VkExtent2D extent) {

    // PIPELINE LAYOUT
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &m_edgeDescriptorLayout;

    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_edgePipelineLayout));

    // SHADERS
    std::string filename;

    filename = "smaa_edge_vert.spv";
    auto vertCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    filename = "smaa_edge_frag.spv";
    auto fragCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    VkShaderModule vertShader = CreateShaderModule(device, vertCode);
    VkShaderModule fragShader = CreateShaderModule(device, fragCode);

    // SHADER STAGES
    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShader;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragShader;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = {
        vertStage,
        fragStage
    };

    // VERTEX INPUT
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // INPUT ASSEMBLY
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // VIEWPORT
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // RASTERIZER
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    // MULTISAMPLING
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // COLOR BLENDING
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // DEPTH
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;

    // PIPELINE
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = m_edgePipelineLayout;
    pipelineInfo.renderPass = m_edgeRenderPass;
    pipelineInfo.subpass = 0;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_edgePipeline));

    // CLEANUP
    vkDestroyShaderModule(device, vertShader, nullptr);
    vkDestroyShaderModule(device, fragShader, nullptr);

}

void VulkanSMAARenderPass::CreateBlendRenderPass(VkDevice device) {

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_blendTarget.Format;
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
        m_blendTarget.View
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
    //m_blendDescriptor.CreateColor(device, desc.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_blendDescriptorLayout = m_blendDescriptor.GetLayout();

}

void VulkanSMAARenderPass::CreateBlendPipeline(VkDevice device, VkExtent2D extent) {

    // PIPELINE LAYOUT
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &m_blendDescriptorLayout;

    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_blendPipelineLayout));

    // SHADERS
    std::string filename;

    filename = "smaa_blend_vert.spv";
    auto vertCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    filename = "smaa_blend_frag.spv";
    auto fragCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    VkShaderModule vertShader = CreateShaderModule(device, vertCode);
    VkShaderModule fragShader = CreateShaderModule(device, fragCode);

    // SHADER STAGES
    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShader;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragShader;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = {
        vertStage,
        fragStage
    };

    // VERTEX INPUT
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // INPUT ASSEMBLY
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // VIEWPORT
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // RASTERIZER
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    // MULTISAMPLING
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // COLOR BLENDING
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // DEPTH
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;

    // PIPELINE
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = m_blendPipelineLayout;
    pipelineInfo.renderPass = m_blendRenderPass;
    pipelineInfo.subpass = 0;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_blendPipeline));

    // CLEANUP
    vkDestroyShaderModule(device, vertShader, nullptr);
    vkDestroyShaderModule(device, fragShader, nullptr);

}

void VulkanSMAARenderPass::CreateNeighborhoodRenderPass(VkDevice device) {

    // COLOR ATTACHMENT
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_outputColor->Format;
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

void VulkanSMAARenderPass::CreateNeighborhoodFramebuffer(VkDevice device, VkExtent2D extent) {

    // ATTACHMENTS
    VkImageView attachments[] = {
        m_outputColor->View
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
    //m_neighborhoodDescriptor.CreateColor(device, desc.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_neighborhoodDescriptorLayout = m_neighborhoodDescriptor.GetLayout();

}

void VulkanSMAARenderPass::CreateNeighborhoodPipeline(VkDevice device, VkExtent2D extent) {

    // PIPELINE LAYOUT
    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &m_neighborhoodDescriptorLayout;

    VK_CHECK(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_neighborhoodPipelineLayout));

    // SHADERS
    std::string filename;

    filename = "smaa_neighborhood_vert.spv";
    auto vertCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    filename = "smaa_neighborhood_frag.spv";
    auto fragCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    VkShaderModule vertShader = CreateShaderModule(device, vertCode);
    VkShaderModule fragShader = CreateShaderModule(device, fragCode);

    // SHADER STAGES
    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShader;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragShader;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = {
        vertStage,
        fragStage
    };

    // VERTEX INPUT
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // INPUT ASSEMBLY
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // VIEWPORT
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // RASTERIZER
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    // MULTISAMPLING
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // COLOR BLENDING
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // DEPTH
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;

    // PIPELINE
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.layout = m_neighborhoodPipelineLayout;
    pipelineInfo.renderPass = m_neighborhoodRenderPass;
    pipelineInfo.subpass = 0;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_neighborhoodPipeline));

    // CLEANUP
    vkDestroyShaderModule(device, vertShader, nullptr);
    vkDestroyShaderModule(device, fragShader, nullptr);

}
#include "pch.h"
#include "VulkanSceneRenderPass.h"
#include "Debug/ErrorDialog.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"

void VulkanSceneRenderPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, VkFormat depthFormat, AntiAliasing aaMode, VkSampleCountFlagBits samples, ApplicationDesc& desc) {

    std::vector<VkAttachmentDescription> attachments;

    VkAttachmentReference colorRef{};
    VkAttachmentReference depthRef{};
    VkAttachmentReference resolveRef{};
    VkSubpassDescription subpass{};

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

    // RESOLVE (final image → shader)
    VkAttachmentDescription resolve{};
    resolve.format = colorFormat;
    resolve.samples = VK_SAMPLE_COUNT_1_BIT;
    resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    resolve.finalLayout   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    attachments.push_back(msaa);            // 0
    attachments.push_back(depth);           // 1
    attachments.push_back(resolve);         // 2

    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    resolveRef.attachment = 2;
    resolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // SUBPASS
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
    VkExtent2D ssaaExtent = renderExtent;
    ssaaExtent.width *= desc.SSAA_SCALE;
    ssaaExtent.height *= desc.SSAA_SCALE;
    m_renderExtent = ssaaExtent;
    m_color.Create(device, physicalDevice, ssaaExtent.width, ssaaExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, samples);
    m_depth.Create(device, physicalDevice, ssaaExtent.width, ssaaExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, samples);
    m_resolve.Create(device, physicalDevice, ssaaExtent.width, ssaaExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // FRAMEBUFFER
    std::vector<VkImageView> imageViewAttachments;
    imageViewAttachments.push_back(m_color.GetImageView());
    imageViewAttachments.push_back(m_depth.GetImageView());
    imageViewAttachments.push_back(m_resolve.GetImageView());

    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = m_renderPass;
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(imageViewAttachments.size());
    framebufferCreateInfo.pAttachments = imageViewAttachments.data();
    framebufferCreateInfo.width = ssaaExtent.width;
    framebufferCreateInfo.height = ssaaExtent.height;
    framebufferCreateInfo.layers = 1;

    VK_CHECK(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_framebuffer));

    // *******************************************************
    // ****************** PIPELINE ***************************
    // *******************************************************
    std::string filename;

    filename = "scene_vert.spv";
    auto vertCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    filename = "scene_frag.spv";
    auto fragCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    VkShaderModule vertModule = CreateShaderModule(device, vertCode);
    VkShaderModule fragModule = CreateShaderModule(device, fragCode);

    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertModule;
    vertStage.pName = "main";

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStage.module = fragModule;
    fragStage.pName = "main";

    VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

    // VERTEX INPUT
    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexBindingDescriptionCount = 0;
    vertexInput.vertexAttributeDescriptionCount = 0;

    // INPUT ASSEMBLY
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // VIEWPORT / SCISSOR (dynamic)
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // RASTERIZER
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;

    // MULTISAMPLING
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    if (aaMode == AntiAliasing::MSAA || aaMode == AntiAliasing::MSAA_SMAA) {
        multisampling.rasterizationSamples = samples;
    } else {
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    }

    // DEPTH TEST
    VkPipelineDepthStencilStateCreateInfo depthStencilState{};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.stencilTestEnable = VK_FALSE;

    // COLOR BLEND
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.logicOpEnable = VK_FALSE;
    colorBlend.logicOp = VK_LOGIC_OP_COPY;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments = &colorBlendAttachment;
    colorBlend.blendConstants[0] = 0.0f;
    colorBlend.blendConstants[1] = 0.0f;
    colorBlend.blendConstants[2] = 0.0f;
    colorBlend.blendConstants[3] = 0.0f;

    // PIPELINE LAYOUT
    VkPipelineLayoutCreateInfo layout{};
    layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout.setLayoutCount = 0;
    layout.pSetLayouts = nullptr;
    layout.pushConstantRangeCount = 0;
    layout.pPushConstantRanges = nullptr;

    VK_CHECK(vkCreatePipelineLayout(device, &layout, nullptr, &m_pipelineLayout));

    // CREATE PIPELINE
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &colorBlend;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));;

    vkDestroyShaderModule(device, vertModule, nullptr);
    vkDestroyShaderModule(device, fragModule, nullptr);

    std::cout << "[Vulkan] Scene-render pass created" << std::endl;

}

void VulkanSceneRenderPass::Destroy(VkDevice device) {

    vkDestroyPipeline(device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);

    vkDestroyFramebuffer(device, m_framebuffer, nullptr);
    m_framebuffer = VK_NULL_HANDLE;

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

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

}

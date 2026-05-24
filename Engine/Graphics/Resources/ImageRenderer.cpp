#include "ImageRenderer.h"
#include "Debug/ErrorDialog.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"

void ImageRenderer::Create(VkDevice device, VkRenderPass renderPass) {

    m_device = device;
    m_renderPass = renderPass;

    CreateDescriptorSetLayout();
    CreateDescriptorPool();
    CreateDescriptorSet();
    CreatePipelineLayout();
    CreatePipeline();

}

void ImageRenderer::Shutdown() {

    if (m_pipeline != VK_NULL_HANDLE) {
        vkDestroyPipeline(m_device, m_pipeline, nullptr);
        m_pipeline = VK_NULL_HANDLE;
    }

    if (m_pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    if (m_descriptorSet != VK_NULL_HANDLE) {
        // Brak destroy
        m_descriptorSet = VK_NULL_HANDLE;
    }

    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }

    if (m_descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(m_device, m_descriptorSetLayout, nullptr);
        m_descriptorSetLayout = VK_NULL_HANDLE;
    }

    if (m_image) {
        m_image->Shutdown();
        delete m_image;
        m_image = nullptr;
    }

    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }

}

void ImageRenderer::Render(VkCommandBuffer commandBuffer, fvec2 screenSize, const Camera& camera) {

    m_commandBuffer = commandBuffer;

    vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);

    UIPushConstants push{};

    push.projection = camera.GetProjection();
    push.position = m_image->GetPosition();
    push.size = m_image->GetSize();

    vkCmdPushConstants(m_commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(UIPushConstants), &push);

    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSet, 0, nullptr);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = screenSize.x;
    viewport.height = screenSize.y;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = {
        static_cast<uint32_t>(screenSize.x),
        static_cast<uint32_t>(screenSize.y)
    };

    vkCmdSetScissor(m_commandBuffer, 0, 1, &scissor);

    vkCmdDraw(m_commandBuffer, 4, 1, 0, 0);

}

void ImageRenderer::CreateDescriptorSetLayout() {

    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    binding.descriptorCount = 1;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.bindingCount = 1;
    info.pBindings = &binding;

    VK_CHECK(vkCreateDescriptorSetLayout(m_device, &info, nullptr, &m_descriptorSetLayout));

}

void ImageRenderer::CreateDescriptorPool() {

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSize.descriptorCount = 1;

    VkDescriptorPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    info.poolSizeCount = 1;
    info.pPoolSizes = &poolSize;
    info.maxSets = 1;

    VK_CHECK(vkCreateDescriptorPool(m_device, &info, nullptr, &m_descriptorPool));

}

void ImageRenderer::CreateDescriptorSet() {

    VkDescriptorSetAllocateInfo alloc{};
    alloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc.descriptorPool = m_descriptorPool;
    alloc.descriptorSetCount = 1;
    alloc.pSetLayouts = &m_descriptorSetLayout;

    VK_CHECK(vkAllocateDescriptorSets(m_device, &alloc, &m_descriptorSet));

}

void ImageRenderer::UpdateDescriptor() {

    if (!m_image) return;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView   = m_image->GetTexture().GetImageView();
    imageInfo.sampler     = m_image->GetTexture().GetSampler();

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSet;
    write.dstBinding = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);

}

void ImageRenderer::CreatePipeline() {

    // SHADERS
    std::string filename;

    filename = "image_vert.spv";
    auto vertCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    filename = "image_frag.spv";
    auto fragCode = ReadFile("../Assets/Shaders/" + filename);
    std::cout << "[Shader] Loading: " << filename << std::endl;

    VkShaderModule vertShader = CreateShaderModule(m_device, vertCode);
    VkShaderModule fragShader = CreateShaderModule(m_device, fragCode);

    // Shader stages
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

    VkPipelineShaderStageCreateInfo stages[] = { vertStage, fragStage };

    // Vertex input (nie ma)
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport state
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Color blending
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType =VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    // Depth stencil
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_ALWAYS;

    // Pipeline create
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.pDynamicState = &dynamicState;

    VK_CHECK(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline));

    // Cleanup
    vkDestroyShaderModule(m_device, vertShader, nullptr);
    vkDestroyShaderModule(m_device, fragShader, nullptr);

}

void ImageRenderer::SetImage(Image& image) {

    m_image = &image;
    UpdateDescriptor();

}

void ImageRenderer::CreatePipelineLayout() {

    VkPushConstantRange pushRange{};
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushRange.offset = 0;
    pushRange.size = sizeof(UIPushConstants);

    VkPipelineLayoutCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    info.setLayoutCount = 1;
    info.pSetLayouts = &m_descriptorSetLayout;
    info.pushConstantRangeCount = 1;
    info.pPushConstantRanges = &pushRange;

    VK_CHECK(vkCreatePipelineLayout(m_device, &info, nullptr, &m_pipelineLayout));

}


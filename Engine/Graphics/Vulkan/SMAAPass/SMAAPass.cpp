#include "pch.h"
#include "SMAAPass.h"
#include "Core/Settings.h"
#include "Graphics/Vulkan/Utils/VulkanUtils.h"
#include "ThirdParty/smaa_textures/AreaTex.h"
#include "ThirdParty/smaa_textures/SearchTex.h"
#include "Graphics/Vulkan/Wrappers/Buffer.h"

void SMAAPass::Create(VkDevice device, VkPhysicalDevice physicalDevice, VkExtent2D renderExtent, VkFormat colorFormat, Settings& settings, VkCommandPool commandPool, VkQueue graphicsQueue) {

    m_device = device;
    m_commandPool = commandPool;
    m_graphicsQueue = graphicsQueue;


    // AREA TEXTURE
    m_areaTexture.Create(device, physicalDevice, 160, 560, VK_FORMAT_R8G8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // AREA STAGING BUFFER
    VkDeviceSize areaSize = sizeof(areaTexBytes);
    Buffer areaStagingBuffer;
    areaStagingBuffer.Create(device, physicalDevice, areaSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // COPY AREA DATA
    void* data = areaStagingBuffer.Map();
    memcpy(data, areaTexBytes, static_cast<size_t>(areaSize));
    areaStagingBuffer.Unmap();
    TransitionImageLayoutImmediate(device, m_commandPool, m_graphicsQueue, m_areaTexture.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(device, m_commandPool, m_graphicsQueue, areaStagingBuffer.Get(), m_areaTexture.GetImage(), m_areaTexture.GetWidth(), m_areaTexture.GetHeight());
    TransitionImageLayoutImmediate(device, m_commandPool, m_graphicsQueue, m_areaTexture.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    areaStagingBuffer.Destroy();
    m_areaTexture.SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    // SEARCH TEXTURE
    m_searchTexture.Create(device, physicalDevice, 64, 16, VK_FORMAT_R8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // SEARCH STAGING BUFFER
    VkDeviceSize searchSize = sizeof(searchTexBytes);
    Buffer searchStagingBuffer;
    searchStagingBuffer.Create(device, physicalDevice, searchSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // COPY SEARCH DATA
    data = searchStagingBuffer.Map();
    memcpy(data, searchTexBytes, static_cast<size_t>(searchSize));
    searchStagingBuffer.Unmap();
    TransitionImageLayoutImmediate(device, m_commandPool, m_graphicsQueue, m_searchTexture.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(device, m_commandPool, m_graphicsQueue, searchStagingBuffer.Get(), m_searchTexture.GetImage(), m_searchTexture.GetWidth(), m_searchTexture.GetHeight());
    TransitionImageLayoutImmediate(device, m_commandPool, m_graphicsQueue, m_searchTexture.GetImage(), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    searchStagingBuffer.Destroy();
    m_searchTexture.SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


    // EDGE TARGET
    m_edgeColor.Create(device, physicalDevice, renderExtent.width, renderExtent.height, VK_FORMAT_R8G8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // BLEND TARGET
    m_blendColor.Create(device, physicalDevice, renderExtent.width, renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // OUTPUT TARGET
    m_color.Create(device, physicalDevice, renderExtent.width, renderExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);


    // EDGE PASS
    CreateEdgeDescriptors(device, settings);
    CreateEdgePipeline(device, renderExtent);

    // BLEND PASS
    CreateBlendDescriptors(device, settings);
    CreateBlendPipeline(device, renderExtent);

    // NEIGHBORHOOD PASS
    CreateNeighborhoodDescriptors(device, settings);
    CreateNeighborhoodPipeline(device, renderExtent);

    std::cout << "[Vulkan] SMAA-render pass created" << std::endl;

}

void SMAAPass::Render(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame, RenderTarget& inputColor) {

    // EDGE
    m_edgeDescriptor.UpdateTexture(currentFrame, 0, inputColor.GetImageView(), inputColor.GetLinearSampler());
    RenderEdgePass(commandBuffer, extent, currentFrame);

    // BLEND
    m_blendDescriptor.UpdateTexture(currentFrame, 1, m_edgeColor.GetImageView(), m_edgeColor.GetLinearSampler());
    m_blendDescriptor.UpdateTexture(currentFrame, 2, m_areaTexture.GetImageView(), m_areaTexture.GetLinearSampler());
    m_blendDescriptor.UpdateTexture(currentFrame, 3, m_searchTexture.GetImageView(), m_searchTexture.GetLinearSampler());
    RenderBlendPass(commandBuffer, extent, currentFrame);

    // NEIGHBORHOOD
    m_neighborhoodDescriptor.UpdateTexture(currentFrame, 0, inputColor.GetImageView(), inputColor.GetLinearSampler());
    m_neighborhoodDescriptor.UpdateTexture(currentFrame, 4, m_blendColor.GetImageView(), m_blendColor.GetLinearSampler());
    RenderNeighborhoodPass(commandBuffer, extent, currentFrame);

}

void SMAAPass::Destroy(VkDevice device) {

    // DESCRIPTORS
    m_edgeDescriptor.Destroy();
    m_blendDescriptor.Destroy();
    m_neighborhoodDescriptor.Destroy();
    m_color.Destroy(device);

    // EDGE PASS
    m_edgePipeline.Destroy(device);

    // BLEND PASS
    m_blendPipeline.Destroy(device);

    // NEIGHBORHOOD PASS
    m_neighborhoodPipeline.Destroy(device);

    // TARGETS
    m_edgeColor.Destroy(device);
    m_blendColor.Destroy(device);

    // LOOKUP TEXTURES
    m_areaTexture.Destroy(device);
    m_searchTexture.Destroy(device);

    std::cout << "[Vulkan] SMAA-render pass destroyed" << std::endl;

}

void SMAAPass::RenderEdgePass(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_edgeColor.GetImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 0.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    TransitionImageLayout(commandBuffer, m_edgeColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    // PIPELINE
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_edgePipeline.Get());

    // DESCRIPTORS
    VkDescriptorSet descriptorSet = m_edgeDescriptor.GetSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_edgePipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRendering(commandBuffer);

    TransitionImageLayout(commandBuffer, m_edgeColor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void SMAAPass::RenderBlendPass(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_blendColor.GetImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 0.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    TransitionImageLayout(commandBuffer, m_blendColor, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    // PIPELINE
    vkCmdBindPipeline(commandBuffer,VK_PIPELINE_BIND_POINT_GRAPHICS, m_blendPipeline.Get());

    // DESCRIPTORS
    VkDescriptorSet descriptorSet = m_blendDescriptor.GetSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_blendPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRendering(commandBuffer);

    TransitionImageLayout(commandBuffer, m_blendColor, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void SMAAPass::RenderNeighborhoodPass(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t currentFrame) {

    VkRenderingAttachmentInfo colorAttachment{};
    colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachment.imageView = m_color.GetImageView();
    colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.renderArea.offset = {0, 0};
    renderingInfo.renderArea.extent = extent;
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachment;

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

    vkCmdBeginRendering(commandBuffer, &renderingInfo);

    // PIPELINE
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_neighborhoodPipeline.Get());

    // DESCRIPTORS
    VkDescriptorSet descriptorSet = m_neighborhoodDescriptor.GetSet(currentFrame);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_neighborhoodPipeline.GetLayout(), 0, 1, &descriptorSet, 0, nullptr);

    // DRAW FULLSCREEN TRIANGLE
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    // END
    vkCmdEndRendering(commandBuffer);

    TransitionImageLayout(commandBuffer, m_color, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void SMAAPass::CreateEdgeDescriptors(VkDevice device, Settings& settings) {

    // DESCRIPTORS
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding colorBinding{};
    colorBinding.binding = 0;
    colorBinding.descriptorCount = 1;
    colorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(colorBinding);

    m_edgeDescriptor.Create(device, bindings, settings.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_edgeDescriptorLayout = m_edgeDescriptor.GetLayout();

}

void SMAAPass::CreateEdgePipeline(VkDevice device, VkExtent2D extent) {

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

    PipelineDesc pdesc;
    pdesc.colorFormat = m_edgeColor.GetFormat();
    pdesc.depthFormat = VK_FORMAT_UNDEFINED;
    pdesc.descriptorLayout = m_edgeDescriptorLayout;
    pdesc.vertexShader = "../Engine/Graphics/Vulkan/SMAAPass/Shaders/smaa_edge_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Vulkan/SMAAPass/Shaders/smaa_edge_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;
    pdesc.vertSpec = &specializationInfo;
    pdesc.fragSpec = &specializationInfo;

    m_edgePipeline.Create(device, pdesc);

}

void SMAAPass::CreateBlendDescriptors(VkDevice device, Settings& settings) {

    // DESCRIPTORS
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding edgesBinding{};
    edgesBinding.binding = 1;
    edgesBinding.descriptorCount = 1;
    edgesBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    edgesBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(edgesBinding);

    VkDescriptorSetLayoutBinding areaBinding{};
    areaBinding.binding = 2;
    areaBinding.descriptorCount = 1;
    areaBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    areaBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(areaBinding);

    VkDescriptorSetLayoutBinding searchBinding{};
    searchBinding.binding = 3;
    searchBinding.descriptorCount = 1;
    searchBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    searchBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(searchBinding);

    m_blendDescriptor.Create(device, bindings, settings.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_blendDescriptorLayout = m_blendDescriptor.GetLayout();

}

void SMAAPass::CreateBlendPipeline(VkDevice device, VkExtent2D extent) {

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

    PipelineDesc pdesc;
    pdesc.colorFormat = m_blendColor.GetFormat();
    pdesc.depthFormat = VK_FORMAT_UNDEFINED;
    pdesc.descriptorLayout = m_blendDescriptorLayout;
    pdesc.vertexShader = "../Engine/Graphics/Vulkan/SMAAPass/Shaders/smaa_blend_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Vulkan/SMAAPass/Shaders/smaa_blend_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;
    pdesc.vertSpec = &specializationInfo;
    pdesc.fragSpec = &specializationInfo;

    m_blendPipeline.Create(device, pdesc);

}

void SMAAPass::CreateNeighborhoodDescriptors(VkDevice device, Settings& settings) {

    // DESCRIPTORS
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    VkDescriptorSetLayoutBinding colorBinding{};
    colorBinding.binding = 0;
    colorBinding.descriptorCount = 1;
    colorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    colorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(colorBinding);

    VkDescriptorSetLayoutBinding blendBinding{};
    blendBinding.binding = 4;
    blendBinding.descriptorCount = 1;
    blendBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    blendBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings.push_back(blendBinding);

    m_neighborhoodDescriptor.Create(device, bindings, settings.MAX_FRAMES_IN_FLIGHT);

    // LAYOUT
    m_neighborhoodDescriptorLayout = m_neighborhoodDescriptor.GetLayout();

}

void SMAAPass::CreateNeighborhoodPipeline(VkDevice device, VkExtent2D extent) {

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

    PipelineDesc pdesc;
    pdesc.colorFormat = m_color.GetFormat();
    pdesc.depthFormat = VK_FORMAT_UNDEFINED;
    pdesc.descriptorLayout = m_neighborhoodDescriptorLayout;
    pdesc.vertexShader = "../Engine/Graphics/Vulkan/SMAAPass/Shaders/smaa_neighborhood_vert.spv";
    pdesc.fragmentShader = "../Engine/Graphics/Vulkan/SMAAPass/Shaders/smaa_neighborhood_frag.spv";
    pdesc.depthTest = false;
    pdesc.blending = false;
    pdesc.vertSpec = &specializationInfo;
    pdesc.fragSpec = &specializationInfo;

    m_neighborhoodPipeline.Create(device, pdesc);

}
#pragma once
#include "pch.h"

struct PipelineDesc {
    VkRenderPass renderPass = VK_NULL_HANDLE;

    VkFormat colorFormat = VK_FORMAT_UNDEFINED;
    VkFormat depthFormat = VK_FORMAT_UNDEFINED;

    VkDescriptorSetLayout descriptorLayout = VK_NULL_HANDLE;
    VkPushConstantRange* pushConstants = nullptr;
    VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
    std::string vertexShader;
    std::string fragmentShader;
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
    bool depthTest = true;
    bool depthWrite = true;
    bool blending = false;
    VkSpecializationInfo* vertSpec = nullptr;
    VkSpecializationInfo* fragSpec = nullptr;
    VkVertexInputBindingDescription* bindingDescription = nullptr;
    VkVertexInputAttributeDescription* attributeDescriptions = nullptr;
    uint32_t attributeCount = 0;


};

class Pipeline {
public:

    void Create(
        VkDevice device,
        const PipelineDesc& desc
    );

    void Destroy(VkDevice device);

    [[nodiscard]] VkPipeline Get() const { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout GetLayout() const { return m_layout; }
    VkPipeline* GetHandle() { return &m_pipeline; }
    VkPipelineLayout* GetLayoutHandle() { return &m_layout; }

private:

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_layout = VK_NULL_HANDLE;

};


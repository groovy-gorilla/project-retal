#pragma once
#include "pch.h"

class Pipeline {
public:

    void Create(
        VkDevice device,
        VkRenderPass renderPass,
        VkDescriptorSetLayout descriptorLayout,
        VkPushConstantRange* pushConstant,
        VkSampleCountFlagBits samples,
        const std::string& vertPath,
        const std::string& fragPath,
        bool depthTest,
        bool blending,
        VkSpecializationInfo* vertSpec = nullptr,
        VkSpecializationInfo* fragSpec = nullptr
    );

    void Destroy(VkDevice device);

    [[nodiscard]] VkPipeline Get() const { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout GetLayout() const { return m_layout; }

private:

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_layout = VK_NULL_HANDLE;

};


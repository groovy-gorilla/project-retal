#pragma once
#include "Model.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

struct ModelPushConstants {
    lina::fmat4 mvp;
};

class ModelRenderer {
public:
    void Create(
        VkDevice device,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkSampleCountFlagBits samples
    );

    void Destroy();

    void Render(
        VkCommandBuffer commandBuffer,
        Model& model,
        const Camera& camera,
        const lina::Transform& transform
    );

    void RecreatePipeline(VkDevice device, VkSampleCountFlagBits samples);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    Pipeline m_pipeline;
    PipelineDesc m_pDesc;
};

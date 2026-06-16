#pragma once
#include "Terrain.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"

struct TerrainPushConstants {
    lina::fmat4 mvp;
};

class TerrainRenderer {
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
        Terrain& terrain,
        const Camera& camera,
        const lina::Transform& transform
    );

    void RecreatePipeline(VkDevice device, VkSampleCountFlagBits samples);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    Pipeline m_pipeline;
    PipelineDesc m_pDesc;
};

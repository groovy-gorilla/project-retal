#pragma once
#include "Skyquad.h"
#include "Graphics/Camera/Camera.h"
#include "Graphics/Vulkan/Wrappers/Pipeline.h"
#include "Graphics/Vulkan/Wrappers/Buffer.h"

struct SkyquadPushConstants {
    float altitude;
    float pitch;
    float roll;
    float fov;
};

class SkyquadRenderer {
public:
    void Create(
        VkDevice device,
        VkPhysicalDevice physicalDevice,
        VkFormat colorFormat,
        VkFormat depthFormat,
        VkSampleCountFlagBits samples
    );

    void Destroy();

    void Render(
        VkCommandBuffer commandBuffer,
        Skyquad& skyquad,
        const Camera& camera
    );

    void RecreatePipeline(VkDevice device, VkSampleCountFlagBits samples);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    Pipeline m_pipeline;
    PipelineDesc m_pDesc;

    Buffer m_skyBuffer;
    VkDescriptorSetLayout m_descriptorLayout = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
};

#include "pch.h"
#include "VulkanSceneResources.h"
#include "Debug/ErrorDialog.h"


void VulkanSceneResources::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat colorFormat, VkFormat depthFormat, ApplicationDesc& desc, VkRenderPass renderPass) {

    m_samples = desc.MSAA_SAMPLES;
    m_aaMode = desc.AA_MODE;

    VkExtent2D internalExtent = extent;
    VkExtent2D ssaaExtent = extent;
    ssaaExtent.width *= desc.SSAA_SCALE;
    ssaaExtent.height *= desc.SSAA_SCALE;



    // Scene Color
    //SceneColor.Create(device, physicalDevice, ssaaExtent.width, ssaaExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // Scene Depth
    //SceneDepth.Create(device, physicalDevice, ssaaExtent.width, ssaaExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_SAMPLE_COUNT_1_BIT);

    // MSAA Color
    //MSAAColor.Create(device, physicalDevice, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT, m_samples);

    // MSAA Depth
    //MSAADepth.Create(device, physicalDevice, internalExtent.width, internalExtent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, m_samples);

    // MSAA Resolve Color
    //ResolveColor.Create(device, physicalDevice, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // SSAA Color
    SSAAColor.Create(device, physicalDevice, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);

    // SMAA Color
    SMAAColor.Create(device, physicalDevice, internalExtent.width, internalExtent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_SAMPLE_COUNT_1_BIT);


    std::cout << "[Vulkan] Scene-resources created" << std::endl;

}

void VulkanSceneResources::Destroy(VkDevice device) {


    //SceneColor.Destroy(device);
    //SceneDepth.Destroy(device);
    //MSAAColor.Destroy(device);
    //MSAADepth.Destroy(device);
    //ResolveColor.Destroy(device);
    SSAAColor.Destroy(device);
    SMAAColor.Destroy(device);

    std::cout << "[Vulkan] Scene-resources destroyed" << std::endl;

}
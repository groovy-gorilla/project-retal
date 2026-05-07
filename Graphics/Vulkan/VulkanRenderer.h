#pragma once

#include "../../Engine/Core/Window.h"
#include "Core/VulkanInstance.h"
#include "Core/VulkanDebug.h"
#include "Core/VulkanSurface.h"
#include "Core/VulkanPhysicalDevice.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapchain.h"
#include "SceneRendering/VulkanSceneRenderPass.h"
#include "PostRendering/VulkanPostRenderPass.h"
#include "SceneRendering/VulkanScenePipeline.h"
#include "SceneRendering/VulkanSceneResources.h"
#include "PostRendering/VulkanPostResources.h"
#include "Core/VulkanCommands.h"
#include "Core/VulkanSync.h"
#include "Core/VulkanQueues.h"

struct ViewportRect {
    float x;
    float y;
    float width;
    float height;
};


class VulkanRenderer {
public:
    void Initialize(Window& window, ApplicationDesc& desc);
    void Shutdown(ApplicationDesc& desc);

    void RecordCommandBuffer(uint32_t imageIndex, ApplicationDesc& desc);

    void Render(ApplicationDesc& desc);




private:
    VulkanInstance m_instance;
    VulkanDebug m_debug;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VkSampleCountFlagBits m_currentMsaa = VK_SAMPLE_COUNT_1_BIT;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;

    // SCENE
    VulkanSceneRenderPass m_sceneRenderPass;
    VulkanSceneResources m_sceneResources;
    VulkanScenePipeline m_scenePipeline;

    // SCREEN
    VulkanPostRenderPass m_postRenderPass;
    VulkanPostResources m_postResources;

    VulkanCommands m_commands;
    VulkanSync m_sync;
    VulkanQueues m_queues;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;


};


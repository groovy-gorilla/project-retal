#pragma once

#include "Core/Display.h"
#include "Core/Window.h"
#include "Core/Timer.h"
#include "Core/VulkanInstance.h"
#include "Core/VulkanDebug.h"
#include "Core/VulkanSurface.h"
#include "Core/VulkanPhysicalDevice.h"
#include "Core/VulkanDevice.h"
#include "Core/VulkanSwapchain.h"
#include "SceneRenderPass/VulkanSceneRenderPass.h"
#include "PostProcessRenderPass/VulkanPostRenderPass.h"
#include "Core/VulkanCommands.h"
#include "Core/VulkanSync.h"
#include "Core/VulkanQueues.h"
#include "SMAARenderPass/VulkanSMAARenderPass.h"
#include "SSAARenderPass/VulkanSSAARenderPass.h"

struct VulkanContext {
    VkDevice device = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
};

struct ViewportRect {
    float x;
    float y;
    float width;
    float height;
};


class VulkanRenderer {
public:
    void Initialize(Display& display, Window& window, ApplicationDesc& desc);
    void Shutdown();

    void RecordCommandBuffer(VkDevice device, uint32_t imageIndex, ApplicationDesc& desc);

    void Update(float deltaTime, bool hdrEnable);
    void Render(VkDevice device, ApplicationDesc& desc);

    void RecreateSwapchain(Display& display, Window& window, ApplicationDesc& desc);
    void RecreateRenderer(Display& display, Window& window, ApplicationDesc& desc);

    void SetTargetExposure(float exposure);

    void TakeScreenshot(uint32_t imageIndex);

    const VulkanContext& GetContext() const { return m_context; }

    VulkanPostRenderPass& GetPostRenderPass() { return m_postRenderPass; }
    VulkanSSAARenderPass& GetSSAARenderPass() { return m_ssaaRenderPass; }

private:
    VulkanContext m_context;

    VulkanInstance m_instance;
    VulkanDebug m_debug;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VkSampleCountFlagBits m_currentMsaa = VK_SAMPLE_COUNT_1_BIT;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;
    uint32_t m_currentFrame = 0;

    // SCENE
    VulkanSceneRenderPass m_sceneRenderPass;

    // SCREEN
    VulkanPostRenderPass m_postRenderPass;
    VulkanSMAARenderPass m_smaaRenderPass;
    VulkanSSAARenderPass m_ssaaRenderPass;

    VulkanCommands m_commands;
    VulkanSync m_sync;
    VulkanQueues m_queues;

    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    // EXTENT
    VkExtent2D m_renderExtent = {};
    VkExtent2D m_ssaaExtent = {};
    VkExtent2D m_windowExtent = {};

    // HDR
    VkFormat m_hdrFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
    float m_exposure = 1.0f;
    float m_targetExposure = 1.0f;

    void UpdateExposure(float deltaTime, bool hdrEnable);


};

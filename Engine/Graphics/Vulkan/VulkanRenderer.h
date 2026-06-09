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
#include "OverlayRenderPass/VulkanOverlayRenderPass.h"
#include "PresentPass/VulkanPresentRenderPass.h"
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
    void Initialize(Display& display, Window& window, Settings& settings);
    void Shutdown();

    void BeginFrame();
    void BeginScene();
    void EndScene();
    void BeginOverlay(Settings& settings);
    void EndOverlay();
    void RenderPresent(Settings& settings);
    void EndFrame(Settings& settings);

    void Update(float deltaTime, bool hdrEnable);

    void RecreateSwapchain(Display& display, Window& window, Settings& settings);
    void RecreateRenderer(Display& display, Window& window, Settings& settings);

    void SetTargetExposure(float exposure);

    void TakeScreenshot(uint32_t imageIndex);

    VulkanContext GetContext() const { return m_context; }

    VkCommandBuffer GetCommandBuffer() const { return m_currentCommandBuffer; }

    VkExtent2D GetRenderExtent() const { return m_renderExtent; }

    VulkanSync GetSync() const { return m_sync; }

private:

    VulkanInstance m_instance;
    VulkanDebug m_debug;
    VulkanSurface m_surface;
    VulkanPhysicalDevice m_physicalDevice;
    VkSampleCountFlagBits m_currentMsaa = VK_SAMPLE_COUNT_1_BIT;
    VulkanDevice m_device;
    VulkanSwapchain m_swapchain;
    uint32_t m_currentFrame = 0;
    VulkanContext m_context;

    // SCENE
    VulkanSceneRenderPass m_sceneRenderPass;

    // SCREEN
    VulkanSMAARenderPass m_smaaRenderPass;
    VulkanSSAARenderPass m_ssaaRenderPass;
    VulkanPostRenderPass m_postRenderPass;
    VulkanOverlayRenderPass m_overlayRenderPass;
    VulkanPresentRenderPass m_presentRenderPass;

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

    uint32_t m_imageIndex = 0;
    VkCommandBuffer m_currentCommandBuffer = VK_NULL_HANDLE;
    RenderTarget* m_currentColor = nullptr;

};

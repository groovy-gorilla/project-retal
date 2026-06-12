#include "pch.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image_write.h"
#include "VulkanRenderer.h"

#include "Utils/VulkanUtils.h"
#include "Debug/ErrorDialog.h"
#include "Wrappers/Buffer.h"


void VulkanRenderer::Initialize(Display& display, Window& window, Settings& settings) {

    // ROZDZIELCZOŚĆ RENDERINGU
    m_renderExtent = window.GetRenderExtent(settings);

    // ROZDZIELCZOŚĆ OKNA
    m_windowExtent = window.GetWindowExtent(display, settings);

    std::cout << "Rendering resolution: " << m_renderExtent.width << "x" << m_renderExtent.height << std::endl;
    std::cout << "Window resolution: " << m_windowExtent.width << "x" << m_windowExtent.height << std::endl;

    // CORE
    m_instance.Create();
    m_debug.Create(m_instance.Get());
    m_surface.Create(m_instance.Get(), window.GetHandle());
    m_physicalDevice.Pick(m_instance.Get(), m_surface.Get());
    m_physicalDevice.CreateSupportedSampleCounts();
    m_device.Create(m_physicalDevice.Get(), m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_queues.Create(m_device.Get(), m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily());
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), m_windowExtent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily(), settings.VSYNC);
    m_commands.Create(m_device.Get(), m_physicalDevice.GetGraphicsQueueFamily(), static_cast<uint32_t>(m_swapchain.GetImages().size()));
    m_sync.Create(m_device.Get(), settings.MAX_FRAMES_IN_FLIGHT);

    // VULKAN CONTEXT
    m_context.device = m_device.Get();
    m_context.physicalDevice = m_physicalDevice.Get();
    m_context.commandPool = m_commands.GetPool();
    m_context.graphicsQueue = m_queues.GetGraphics();

    // SSAA UPSCALE CALCULATION
    m_ssaaExtent.width = m_renderExtent.width * settings.SSAA_SCALE;
    m_ssaaExtent.height = m_renderExtent.height * settings.SSAA_SCALE;

    // SCENE
    m_sceneRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_ssaaExtent, m_hdrFormat, FindDepthFormat(m_physicalDevice.Get()), settings.AA_MODE, settings.MSAA_SAMPLES, settings);

    // SMAA
    m_smaaRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_hdrFormat, settings, m_commands.GetPool(), m_queues.GetGraphics());

    // SSAA
    m_ssaaRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_hdrFormat, /*IN*/m_sceneRenderPass.GetColor(), settings);

    // POST
    m_postRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_hdrFormat, settings);

    // OVERLAY
    m_overlayRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_swapchain.GetImageFormat(), settings);

    // PRESENT
    m_presentRenderPass.Create(m_device.Get(), m_swapchain.GetImageFormat(), settings);

}

void VulkanRenderer::Shutdown() {

    vkDeviceWaitIdle(m_device.Get());

    m_stagingBuffer.Destroy();

    m_presentRenderPass.Destroy();
    m_overlayRenderPass.Destroy(m_device.Get());
    m_postRenderPass.Destroy(m_device.Get());
    m_ssaaRenderPass.Destroy(m_device.Get());
    m_smaaRenderPass.Destroy(m_device.Get());
    m_sceneRenderPass.Destroy(m_device.Get());
    m_sync.Destroy(m_device.Get());
    m_commands.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());
    m_device.Destroy();
    m_surface.Destroy(m_instance.Get());
    m_debug.Destroy(m_instance.Get());
    m_instance.Destroy();

}

void VulkanRenderer::BeginFrame() {

    m_sync.Wait(m_device.Get());

    // ACQUIRE IMAGE
    VK_CHECK(vkAcquireNextImageKHR(m_device.Get(), m_swapchain.Get(), UINT64_MAX, m_sync.GetImageAvailableSemaphore(), VK_NULL_HANDLE, &m_imageIndex));

    VkCommandBuffer cmd = m_commands.Get(m_imageIndex);

    // RESET
    vkResetCommandBuffer(cmd, 0);

    // BEGIN COMMAND BUFFER
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmd, &beginInfo);

    m_currentCommandBuffer = cmd;

}

void VulkanRenderer::BeginScene() {

    // CURRENT PIPELINE OUTPUT
    m_currentColor = &m_sceneRenderPass.GetColor();

    // SCENE PASS
    m_sceneRenderPass.Begin(m_currentCommandBuffer);

}

void VulkanRenderer::EndScene() {

    m_sceneRenderPass.End(m_currentCommandBuffer);

}

void VulkanRenderer::BeginOverlay(Settings& set) {

    // ANTIALIASING LOGIC
    switch(set.AA_MODE) {

        case AntiAliasing::None:
            break;

        case AntiAliasing::MSAA:
            m_currentColor = &m_sceneRenderPass.GetResolve();
            break;

        case AntiAliasing::SMAA:
            m_smaaRenderPass.Render(m_currentCommandBuffer, m_renderExtent, m_sync.GetCurrentFrame(), *m_currentColor);
            m_currentColor = &m_smaaRenderPass.GetColor();
            break;

        case AntiAliasing::SSAA:
            m_ssaaRenderPass.Render(m_currentCommandBuffer, m_renderExtent, m_sync.GetCurrentFrame());
            m_currentColor = &m_ssaaRenderPass.GetColor();
            break;

        case AntiAliasing::MSAA_SMAA:
            m_currentColor = &m_sceneRenderPass.GetResolve();
            m_smaaRenderPass.Render(m_currentCommandBuffer, m_renderExtent, m_sync.GetCurrentFrame(), *m_currentColor);
            m_currentColor = &m_smaaRenderPass.GetColor();
            break;

        case AntiAliasing::SSAA_SMAA:
            m_ssaaRenderPass.Render(m_currentCommandBuffer, m_renderExtent, m_sync.GetCurrentFrame());
            m_currentColor = &m_ssaaRenderPass.GetColor();
            m_smaaRenderPass.Render(m_currentCommandBuffer, m_renderExtent, m_sync.GetCurrentFrame(), *m_currentColor);
            m_currentColor = &m_smaaRenderPass.GetColor();
            break;

        default:
            break;

    }

    // POST PASS
    m_postRenderPass.Render(m_sync.GetCurrentFrame(), m_currentCommandBuffer, *m_currentColor, m_renderExtent, set, m_exposure);
    m_currentColor = &m_postRenderPass.GetColor();

    // OVERLAY PASS
    m_overlayRenderPass.Begin(m_sync.GetCurrentFrame(), m_currentCommandBuffer, *m_currentColor, m_renderExtent, set);
    m_currentColor = &m_overlayRenderPass.GetColor();

}

void VulkanRenderer::EndOverlay() {

    m_overlayRenderPass.End(m_currentCommandBuffer);

}

void VulkanRenderer::RenderPresent(Settings& settings) {

    m_presentRenderPass.Render(m_sync.GetCurrentFrame(), m_currentCommandBuffer, *m_currentColor, m_swapchain, m_imageIndex, m_swapchain.GetExtent(), settings);

}

void VulkanRenderer::EndFrame(Settings& settings) {

    // END COMMAND BUFFER
    vkEndCommandBuffer(m_currentCommandBuffer);

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
    };

    m_sync.Reset(m_device.Get());

    // SUBMIT
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer = m_commands.Get(m_imageIndex);
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 1;
    VkSemaphore availableSemaphore = m_sync.GetImageAvailableSemaphore();
    submitInfo.pWaitSemaphores = &availableSemaphore;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.signalSemaphoreCount = 1;
    VkSemaphore finishedSemaphore = m_sync.GetRenderFinishedSemaphore();
    submitInfo.pSignalSemaphores = &finishedSemaphore;

    VK_CHECK(vkQueueSubmit(m_queues.GetGraphics(), 1, &submitInfo, m_sync.GetFence()));

    // TAKE SCREENSHOT
    if (settings.TAKE_SCREENSHOT) {
        vkQueueWaitIdle(m_queues.GetGraphics());
        TakeScreenshot(m_imageIndex);
        settings.TAKE_SCREENSHOT = false;
    }

    // PRESENT
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSwapchainKHR swapchains[] = {
        m_swapchain.Get()
    };

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_imageIndex;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &finishedSemaphore;

    VK_CHECK(vkQueuePresentKHR(m_queues.GetPresent(), &presentInfo));

    m_sync.NextFrame(settings.MAX_FRAMES_IN_FLIGHT);

}

void VulkanRenderer::Update(float deltaTime, bool hdrEnable) {

    UpdateExposure(deltaTime, hdrEnable);

}

void VulkanRenderer::RecreateSwapchain(Display& display, Window& window, Settings& settings) {

    vkDeviceWaitIdle(m_device.Get());

    // NOWE WINDOW EXTENT
    m_windowExtent = window.GetWindowExtent(display, settings);

    // DESTROY
    m_swapchain.Destroy(m_device.Get());

    // CREATE
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), m_windowExtent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily(), settings.VSYNC);

}

void VulkanRenderer::RecreateRenderer(Display& display, Window& window, Settings& settings) {

    vkDeviceWaitIdle(m_device.Get());

    // DESTROY
    m_presentRenderPass.Destroy();
    m_overlayRenderPass.Destroy(m_device.Get());
    m_postRenderPass.Destroy(m_device.Get());
    m_smaaRenderPass.Destroy(m_device.Get());
    m_ssaaRenderPass.Destroy(m_device.Get());
    m_sceneRenderPass.Destroy(m_device.Get());
    m_swapchain.Destroy(m_device.Get());

    // EXTENTS
    m_renderExtent = window.GetRenderExtent(settings);
    m_windowExtent = window.GetWindowExtent(display, settings);

    // SWAPCHAIN
    m_swapchain.Create(m_physicalDevice.Get(), m_device.Get(), m_surface.Get(), m_windowExtent, m_physicalDevice.GetGraphicsQueueFamily(), m_physicalDevice.GetPresentQueueFamily(), settings.VSYNC);

    // SSAA UPSCALE CALCULATION
    m_ssaaExtent.width = m_renderExtent.width * settings.SSAA_SCALE;
    m_ssaaExtent.height = m_renderExtent.height * settings.SSAA_SCALE;

    // SCENE
    m_sceneRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_ssaaExtent, m_hdrFormat, FindDepthFormat(m_physicalDevice.Get()), settings.AA_MODE, settings.MSAA_SAMPLES, settings);

    // SMAA
    m_smaaRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_hdrFormat, settings, m_commands.GetPool(), m_device.GetGraphicsQueue());

    // SSAA
    m_ssaaRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_hdrFormat, /*IN*/m_sceneRenderPass.GetColor(), settings);

    // POST
    m_postRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_hdrFormat, settings);

    // OVERLAY
    m_overlayRenderPass.Create(m_device.Get(), m_physicalDevice.Get(), m_renderExtent, m_swapchain.GetImageFormat(), settings);

    // PRESENT
    m_presentRenderPass.Create(m_device.Get(), m_swapchain.GetImageFormat(), settings);

}

void VulkanRenderer::UpdateExposure(float deltaTime, bool hdrEnable) {

    if (hdrEnable) {
        m_exposure += (m_targetExposure - m_exposure) * deltaTime * 3.0f;
        //m_exposure = std::lerp(m_exposure, m_targetExposure, deltaTime * 2.0f);
    }
}

void VulkanRenderer::SetTargetExposure(float exposure) {

    m_targetExposure = exposure;

}

void VulkanRenderer::TakeScreenshot(uint32_t imageIndex) {

    VkImage srcImage = m_swapchain.GetImage(imageIndex);

    VkDeviceSize imageSize = static_cast<VkDeviceSize>(m_renderExtent.width) * static_cast<VkDeviceSize>(m_renderExtent.height) * 4;

    // STAGING BUFFER
    Buffer stagingBuffer;
    stagingBuffer.Create(m_device.Get(), m_physicalDevice.Get(), imageSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(m_device.Get(), m_commands.GetPool());

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = srcImage;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { m_renderExtent.width, m_renderExtent.height, 1 };

    vkCmdCopyImageToBuffer(commandBuffer, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, stagingBuffer.Get(), 1, &region);

    VkImageMemoryBarrier barrierBack{};
    barrierBack.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrierBack.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrierBack.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrierBack.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierBack.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrierBack.image = srcImage;
    barrierBack.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrierBack.subresourceRange.baseMipLevel = 0;
    barrierBack.subresourceRange.levelCount = 1;
    barrierBack.subresourceRange.baseArrayLayer = 0;
    barrierBack.subresourceRange.layerCount = 1;
    barrierBack.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrierBack.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrierBack);

    EndSingleTimeCommands(m_device.Get(), m_commands.GetPool(), m_queues.GetGraphics(), commandBuffer);

    void* data = stagingBuffer.Map();

    std::vector<uint8_t> pixels(imageSize);

    // KONWERSJA Z BGRA NA RGBA
    uint8_t* mapped = static_cast<uint8_t*>(data);

    for (size_t i = 0; i < imageSize; i += 4) {
        pixels[i + 0] = mapped[i + 2];
        pixels[i + 1] = mapped[i + 1];
        pixels[i + 2] = mapped[i + 0];
        pixels[i + 3] = mapped[i + 3];
    }

    // KONWERSJA Z RGBA NA RGB
    std::vector<uint8_t> rgb;
    rgb.resize(m_renderExtent.width * m_renderExtent.height * 3);

    for (size_t src = 0, dst = 0; src < imageSize; src += 4) {
        rgb[dst++] = pixels[src + 0]; // R
        rgb[dst++] = pixels[src + 1]; // G
        rgb[dst++] = pixels[src + 2]; // B
    }

    // CREATE DIRECTORY
    std::filesystem::create_directory("Screenshots");

    // TIMESTAMP FILENAME
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm localtime = *std::localtime(&time);
    std::stringstream ss;
    ss << "Screenshots/screenshot_" << std::put_time(&localtime, "%Y-%m-%d_%H-%M-%S") << ".png";
    std::string filename = ss.str();

    // WRITE IMAGE TO PNG FILE
    stbi_write_png(filename.c_str(), m_renderExtent.width, m_renderExtent.height, 3, rgb.data(), m_renderExtent.width * 3);

    // DESTROY
    stagingBuffer.Unmap();
    stagingBuffer.Destroy();

    std::cout << "Saving screenshot: " << filename << std::endl;

}
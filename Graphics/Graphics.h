#pragma once

#include "Vulkan/VulkanRenderer.h"

class Window;

class Graphics {
public:
    void Initialize(Display& display, Window& window, ApplicationDesc& desc);
    void Shutdown(ApplicationDesc& desc);

    void Render(ApplicationDesc& desc);

    void RecreateSwapchain(Window& window);

    VulkanRenderer& GetRenderer();

private:
    VulkanRenderer m_renderer;

};


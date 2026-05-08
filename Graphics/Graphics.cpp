#include "Graphics.h"

void Graphics::Initialize(Display& display, Window& window, ApplicationDesc& desc) {
    m_renderer.Initialize(display, window, desc);
}

void Graphics::Shutdown(ApplicationDesc& desc) {
    m_renderer.Shutdown(desc);
}

void Graphics::Render(ApplicationDesc& desc) {
    m_renderer.Render(desc);
}

void Graphics::RecreateSwapchain(Window& window) {
    //m_renderer.RecreateSwapchain(window);
}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}

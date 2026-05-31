#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize(Display& display, Window& window, ApplicationDesc& desc) {

    m_renderer.Initialize(display, window, desc);

    m_spriteRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass());

    m_sprite.Create(m_renderer.GetContext(), "gruvbox.ktx");
    m_spriteRenderer.SetSprite(m_sprite);


}

void Graphics::Shutdown() {

    m_spriteRenderer.Shutdown();

    m_sprite.Shutdown();

    m_renderer.Shutdown();

}

void Graphics::Render(VkDevice device, ApplicationDesc& desc, float deltaTime) {

    m_renderer.Update(deltaTime, desc.HDR);

    m_renderer.BeginFrame();
    m_renderer.BeginScene();

    // TUTAJ SCENA

    m_renderer.EndScene();
    m_renderer.BeginOverlay(desc);

    // TUTAJ OVERLAY
    auto extent = m_renderer.GetRenderExtent();
    m_camera.SetOrthographic(0.0, extent.width, extent.height, 0.0, -1.0, 1.0);
    m_sprite.SetPosition(0,0);
    m_sprite.SetSize(extent.width, extent.height);
    m_spriteRenderer.Render(m_renderer.GetCommandBuffer(), extent, m_camera);

    m_renderer.EndOverlay();
    m_renderer.RenderPresent(desc);
    m_renderer.EndFrame(desc);

}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}

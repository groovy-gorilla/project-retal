#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize(Display& display, Window& window, ApplicationDesc& desc) {

    m_renderer.Initialize(display, window, desc);

    m_spriteRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass());
    m_msdfTextRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass());

    m_sprite1.Create(m_renderer.GetContext(), "gruvbox.ktx", desc.MAX_FRAMES_IN_FLIGHT);
    m_sprite2.Create(m_renderer.GetContext(), "smile.ktx", desc.MAX_FRAMES_IN_FLIGHT);

    m_msdfFont.Initialize(m_renderer.GetContext(), "Fonts/tfx.ktx", "Fonts/tfx.fda", desc);
    m_msdfText.Initialize(m_renderer.GetContext(), m_msdfFont);
    m_msdfText.SetScale(19.0f);
    m_msdfText.SetPosition(10, 200);
    m_msdfText.SetText("Hello World!");


}

void Graphics::Shutdown() {

    m_spriteRenderer.Shutdown();
    m_msdfTextRenderer.Shutdown();

    m_sprite1.Shutdown();
    m_sprite2.Shutdown();

    m_msdfText.Destroy();

    m_msdfFont.Destroy();

    m_renderer.Shutdown();

}

void Graphics::Render(VkDevice device, ApplicationDesc& desc, float deltaTime) {

    m_renderer.Update(deltaTime, desc.HDR);

    m_renderer.BeginFrame();
    m_renderer.BeginScene();

    // TUTAJ SCENA
    // ...

    m_renderer.EndScene();
    m_renderer.BeginOverlay(desc);

    // TUTAJ OVERLAY
    auto extent = m_renderer.GetRenderExtent();
    m_camera.SetOrthographic(0.0, extent.width, extent.height, 0.0, -1.0, 1.0);

    m_sprite1.SetPosition(0,100);
    m_sprite1.SetSize(extent.width / 2, extent.height);
    //m_spriteRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_sprite1, m_camera);

    m_sprite2.SetPosition(extent.width / 2,0);
    m_sprite2.SetSize(extent.width / 2, extent.height);
    //m_spriteRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_sprite2, m_camera);

    m_msdfTextRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_msdfText, m_camera);

    m_renderer.EndOverlay();
    m_renderer.RenderPresent(desc);
    m_renderer.EndFrame(desc);

}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}

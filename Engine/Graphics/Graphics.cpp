#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize(Display& display, Window& window, Settings& settings) {

    m_renderer.Initialize(display, window, settings);

    //m_spriteRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass());
    //m_textRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass());

    m_sprite1.Create(m_renderer.GetContext(), "gruvbox.ktx", settings.MAX_FRAMES_IN_FLIGHT);
    m_sprite2.Create(m_renderer.GetContext(), "smile.ktx", settings.MAX_FRAMES_IN_FLIGHT);

    m_Font.Initialize(m_renderer.GetContext(), "Fonts/JetBrains.ktx", "Fonts/JetBrains.fda", TextureFilter::Linear, settings);
    m_Text.Initialize(m_renderer.GetContext(), m_Font, 32);
    m_Text.SetSize(15.0f);
    m_Text.SetPosition(10, 10);
    m_Text.SetColor(lina::fvec4(1,1,0,1));

}

void Graphics::Shutdown() {

    //m_spriteRenderer.Shutdown();
    //m_textRenderer.Shutdown();

    m_sprite1.Shutdown();
    m_sprite2.Shutdown();

    m_Text.Destroy();

    m_Font.Destroy();

    m_renderer.Shutdown();

}

void Graphics::Render(VkDevice device, Settings& settings, float deltaTime) {

    m_renderer.Update(deltaTime, settings.HDR);
    m_fps.Update(deltaTime);

    m_renderer.BeginFrame();
    m_renderer.BeginScene();

    // TUTAJ SCENA
    // ...

    m_renderer.EndScene();
    m_renderer.BeginOverlay(settings);

    // TUTAJ OVERLAY
    //auto extent = m_renderer.GetRenderExtent();
    //m_camera.SetOrthographic(0.0, extent.width, extent.height, 0.0, -1.0, 1.0);

    //m_sprite1.SetPosition(0,0);
    //m_sprite1.SetSize(extent.width / 2, extent.height);
    //m_spriteRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_sprite1, m_camera);

    //m_sprite2.SetPosition(extent.width / 2,0);
    //m_sprite2.SetSize(extent.width / 2, extent.height);
    //m_spriteRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_sprite2, m_camera);

    /*std::ostringstream ss;
    ss << "FPS: " << m_fps.GetFPS()
       << "\nFrame: "
       << std::fixed
       << std::setprecision(1)
       << m_fps.GetFrameTime()
       << " ms";
    m_Text.SetText(ss.str());*/
    //m_textRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_Text, m_camera);

    m_renderer.EndOverlay();
    m_renderer.RenderPresent(settings);
    m_renderer.EndFrame(settings);

}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}

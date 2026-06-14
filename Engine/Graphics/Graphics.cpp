#include "pch.h"
#include "Graphics.h"

#include "Input/Input.h"

void Graphics::Initialize(Display& display, Window& window, Settings& set) {

    m_renderer.Initialize(display, window, set);

    m_spriteRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass().GetColorFormat());
    m_textRenderer.Create(m_renderer.GetContext().device, m_renderer.GetOverlayRenderPass().GetColorFormat());
    m_modelRenderer.Create(m_renderer.GetContext().device, m_renderer.GetSceneRenderPass().GetColorFormat(), m_renderer.GetSceneRenderPass().GetDepthFormat(), set.MSAA_SAMPLES);
    m_skydomeRenderer.Create(m_renderer.GetContext().device, m_renderer.GetContext().physicalDevice, m_renderer.GetSceneRenderPass().GetColorFormat(), m_renderer.GetSceneRenderPass().GetDepthFormat(), VK_SAMPLE_COUNT_1_BIT);

    m_skydome.Create(m_renderer.GetContext().device, m_renderer.GetContext().physicalDevice);
    m_skydome.SetSkyColors(SkyPreset::SOMALIA_DAWN);

    m_sprite1.Create(m_renderer.GetContext(), "gruvbox.ktx", set.MAX_FRAMES_IN_FLIGHT);
    m_sprite2.Create(m_renderer.GetContext(), "smile.ktx", set.MAX_FRAMES_IN_FLIGHT);

    m_Font.Initialize(m_renderer.GetContext(), "Fonts/JetBrains.ktx", "Fonts/JetBrains.fda", TextureFilter::Linear, set);
    m_Text.Initialize(m_renderer.GetContext(), m_Font, 32);
    m_Text.SetSize(15.0f);
    m_Text.SetPosition(10, 10);
    m_Text.SetColor(lina::fvec4(1,1,0,1));

    m_cube.CreateCube(m_renderer.GetContext().device, m_renderer.GetContext().physicalDevice);

    m_camera.SetPosition(vec3(0.0, 0.0, 3.0));

}

void Graphics::Shutdown() {

    m_spriteRenderer.Shutdown();
    m_textRenderer.Shutdown();
    m_modelRenderer.Destroy();
    m_skydomeRenderer.Destroy();

    m_skydome.Destroy();

    m_cube.Destroy();

    m_sprite1.Shutdown();
    m_sprite2.Shutdown();

    m_Text.Destroy();

    m_Font.Destroy();

    m_renderer.Shutdown();

}

void Graphics::Render(Settings& set, float deltaTime) {

    m_renderer.Update(deltaTime, set.HDR);
    m_fps.Update(deltaTime);

    m_renderer.BeginFrame();
    m_renderer.BeginScene();

        // TUTAJ SCENA
        auto extent = m_renderer.GetRenderExtent();
        m_camera.SetPerspective(120.0, static_cast<double>(extent.width) / static_cast<double>(extent.height), 0.01, 1000000.0);

        // POSZERZANIE HORYZONTU
        m_skydome.HorizonBroadening(m_camera.GetPosition().y);

        // SKYDOME
        m_skydomeRenderer.Render(m_renderer.GetCommandBuffer(), m_skydome, m_camera);

        // CUBE
        static Transform cubeTransform;
        //cubeTransform.rotation.y += 0.01;
        //cubeTransform.rotation.x += 0.015;
        m_modelRenderer.Render(m_renderer.GetCommandBuffer(), m_cube, m_camera, cubeTransform);



    m_renderer.EndScene();
    m_renderer.BeginOverlay(set);

        // TUTAJ OVERLAY

        extent = m_renderer.GetRenderExtent();
        m_camera.SetOrthographic(0.0, extent.width, extent.height, 0.0, -1.0, 1.0);

        m_sprite1.SetPosition(0,0);
        m_sprite1.SetSize(extent.width / 2, extent.height);
        //m_spriteRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_sprite1, m_camera);

        m_sprite2.SetPosition(extent.width / 2,0);
        m_sprite2.SetSize(extent.width / 2, extent.height);
        //m_spriteRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_sprite2, m_camera);

        std::ostringstream ss;
        ss << "FPS: " << m_fps.GetFPS()
           << "\nFrame: "
           << std::fixed
           << std::setprecision(1)
           << m_fps.GetFrameTime()
           << " ms";
        m_Text.SetText(ss.str());
        m_textRenderer.Render(m_renderer.GetSync().GetCurrentFrame(), m_renderer.GetCommandBuffer(), m_Text, m_camera);

    m_renderer.EndOverlay();
    m_renderer.RenderPresent(set);
    m_renderer.EndFrame(set);

}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}

void Graphics::Recreate(Display& display, Window& window, Settings& set) {

    m_renderer.RecreateRenderer(display, window, set);
    m_modelRenderer.RecreatePipeline(m_renderer.GetContext().device, set.MSAA_SAMPLES);

}

#include "pch.h"
#include "Graphics.h"

void Graphics::Initialize(Display& display, Window& window, ApplicationDesc& desc) {

    m_renderer.Initialize(display, window, desc);

    m_camera.SetOrthographic(0.0, desc.WIDTH, desc.HEIGHT, 0.0, 0.0, 1.0);

    m_image.Create(m_renderer.GetContext(), "gruvbox.ktx");

    m_imageRenderer.Create(m_renderer.GetContext().device, m_renderer.GetPostRenderPass().Get());

}

void Graphics::Shutdown() {

    m_renderer.Shutdown();

}

void Graphics::Render(VkDevice device, ApplicationDesc& desc, float deltaTime) {

    m_renderer.Update(deltaTime, desc.HDR);
    m_renderer.Render(device, desc);

    fvec2 size;
    size.x = desc.WIDTH;
    size.y = desc.HEIGHT;
    m_image.SetSize(size);

    fvec2 position;
    position.x = 0.0f;
    position.y = 0.0f;
    m_image.SetPosition(position);

    m_imageRenderer.SetImage(m_image);
    m_imageRenderer.Render(m_renderer, size, m_camera);

}

VulkanRenderer& Graphics::GetRenderer() {
    return m_renderer;
}

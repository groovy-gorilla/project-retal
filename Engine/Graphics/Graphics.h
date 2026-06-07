#pragma once

#include "Font/Font.h"
#include "Sprite/Sprite.h"
#include "Sprite/SpriteRenderer.h"
#include "Text/Text.h"
#include "Text/TextRenderer.h"
#include "Vulkan/VulkanRenderer.h"

class Display;
class Window;
struct ApplicationDesc;

class Graphics {
public:
    void Initialize(Display& display, Window& window, ApplicationDesc& desc);
    void Shutdown();
    void Render(VkDevice device, ApplicationDesc& desc, float deltaTime);

    VulkanRenderer& GetRenderer();

private:
    VulkanRenderer m_renderer;

    Camera m_camera;

    SpriteRenderer m_spriteRenderer;
    TextRenderer m_textRenderer;

    Sprite m_sprite1;
    Sprite m_sprite2;

    Font m_Font;
    Text m_Text;

};


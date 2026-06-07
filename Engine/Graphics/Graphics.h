#pragma once

#include "Font/Font.h"
#include "Fps/Fps.h"
#include "Sprite/Sprite.h"
#include "Sprite/SpriteRenderer.h"
#include "Text/Text.h"
#include "Text/TextRenderer.h"
#include "Vulkan/VulkanRenderer.h"

class Display;
class Window;
struct Settings;

class Graphics {
public:
    void Initialize(Display& display, Window& window, Settings& settings);
    void Shutdown();
    void Render(VkDevice device, Settings& settings, float deltaTime);

    VulkanRenderer& GetRenderer();

private:
    VulkanRenderer m_renderer;

    Camera m_camera;

    Fps m_fps;

    SpriteRenderer m_spriteRenderer;
    TextRenderer m_textRenderer;

    Sprite m_sprite1;
    Sprite m_sprite2;

    Font m_Font;
    Text m_Text;

};


#pragma once

#include "Font/Font.h"
#include "Fps/Fps.h"
#include "Input/Input.h"
#include "Model/ModelRenderer.h"
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
    void Initialize(Display& display, Window& window, Settings& set);
    void Shutdown();
    void Render(VkDevice device, Settings& settings, float deltaTime, Input& input);

    VulkanRenderer& GetRenderer();

    void Recreate(Display& display, Window& window, Settings& set);

private:
    VulkanRenderer m_renderer;

    Camera m_camera;

    Fps m_fps;

    SpriteRenderer m_spriteRenderer;
    TextRenderer m_textRenderer;
    ModelRenderer m_modelRenderer;

    Model m_model;

    Sprite m_sprite1;
    Sprite m_sprite2;

    Font m_Font;
    Text m_Text;

};


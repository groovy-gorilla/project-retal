#pragma once

#include "Font/Font.h"
#include "Fps/Fps.h"
#include "Input/Input.h"
#include "Model/ModelRenderer.h"
#include "Skydome/SkydomeRenderer.h"
#include "Skyquad/Skyquad.h"
#include "Skyquad/SkyquadRenderer.h"
#include "Sprite/Sprite.h"
#include "Sprite/SpriteRenderer.h"
#include "Terrain/TerrainRenderer.h"
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
    void Render(Settings& set, float deltaTime);

    VulkanRenderer& GetRenderer();

    void Recreate(Display& display, Window& window, Settings& set);

    Camera& GetCamera() { return m_camera; }

private:
    VulkanRenderer m_renderer;

    Camera m_camera;

    Fps m_fps;

    SpriteRenderer m_spriteRenderer;
    TextRenderer m_textRenderer;
    ModelRenderer m_modelRenderer;
    SkyquadRenderer m_skyquadRenderer;
    TerrainRenderer m_terrainRenderer;

    Terrain m_terrain;

    Skyquad m_skyquad;

    Model m_cube;

    Sprite m_sprite1;
    Sprite m_sprite2;

    Font m_Font;
    Text m_Text;

};


#pragma once

#include "Settings.h"
#include "Display.h"

class Window {
public:

    void Create(Settings& settings, Display& display);
    void Destroy();

    [[nodiscard]] bool ShouldClose() const;
    void SetShouldClose(bool value);

    void SetWindowed(Settings& settings, Display& display);
    void SetFullscreen(Settings& settings, Display& display);

    void SetWindowSize(Settings& settings, Display& display);

    // Vulkan
    [[nodiscard]] SDL_Window* GetHandle() const;

    VkExtent2D GetRenderExtent(Settings& settings);
    VkExtent2D GetWindowExtent(Display& display, Settings& settings);

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;

    VkExtent2D m_renderExtent = {};
    VkExtent2D m_windowExtent = {};

};

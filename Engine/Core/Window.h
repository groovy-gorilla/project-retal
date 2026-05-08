#pragma once

#include "ApplicationDesc.h"
#include <SDL3/SDL.h>
#include "Display.h"

class Window {
public:
    Window();
    ~Window();

    void Create(ApplicationDesc& desc, Display& display);
    void Destroy();

    bool ShouldClose() const;
    void SetShouldClose(bool value);

    void SetWindowed(ApplicationDesc& desc, Display& display);
    void SetFullscreen(ApplicationDesc& desc, Uint32 displayID);

    // Vulkan
    SDL_Window* GetHandle() const;

    VkExtent2D GetRenderExtent(ApplicationDesc& desc);
    VkExtent2D GetWindowExtent(Display& display, ApplicationDesc& desc);

private:
    SDL_Window* m_window = nullptr;
    bool m_shouldClose = false;

    VkExtent2D m_renderExtent;
    VkExtent2D m_windowExtent;

};

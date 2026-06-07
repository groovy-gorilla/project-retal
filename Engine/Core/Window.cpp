#include "pch.h"
#include "Window.h"

void Window::Create(Settings &desc, Display& display) {

    setenv("GTK_THEME", "Adwaita:dark", 1);

    m_window = SDL_CreateWindow(
        desc.TITLE,
        desc.WIDTH / display.GetScaling() * desc.WINDOWED_SCALE,
        desc.HEIGHT / display.GetScaling() * desc.WINDOWED_SCALE,
        SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN
    );

    if (!m_window) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_ShowWindow(m_window);
    SDL_SyncWindow(m_window);
    SDL_PumpEvents();

    if (SDL_GetDisplayForWindow(m_window) != SDL_GetPrimaryDisplay()) {
        // tu zmienić położenie na drugi monitor
    }

    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SyncWindow(m_window);
    SDL_SetWindowFullscreenMode(m_window, display.GetCurrentDisplayMode());
    SDL_SetWindowFullscreen(m_window, desc.FULLSCREEN);
    SDL_SyncWindow(m_window);

}

void Window::Destroy() {
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

bool Window::ShouldClose() const {
    return m_shouldClose;
}

void Window::SetShouldClose(bool value) {
    m_shouldClose = value;
}

void Window::SetWindowed(Settings& settings, Display& display) {

    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SyncWindow(m_window);
    SDL_SetWindowFullscreen(m_window, false);
    SDL_SetWindowSize(m_window, settings.WIDTH / display.GetScaling() * settings.WINDOWED_SCALE, settings.HEIGHT / display.GetScaling() * settings.WINDOWED_SCALE);
    SDL_SyncWindow(m_window);

}

void Window::SetFullscreen(Settings& settings, Display& display) {

    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(display.GetCurrentDisplay(), nullptr);

    modes[0]->w = settings.WIDTH;
    modes[0]->h = settings.HEIGHT;

    SDL_SetWindowFullscreenMode(m_window, modes[0]);
    SDL_SetWindowFullscreen(m_window, true);
    SDL_SyncWindow(m_window);

}

void Window::SetWindowSize(Settings& settings, Display& display) {

    SDL_SetWindowSize(m_window, settings.WIDTH / display.GetScaling() * settings.WINDOWED_SCALE, settings.HEIGHT / display.GetScaling() * settings.WINDOWED_SCALE);
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_SyncWindow(m_window);

}

SDL_Window* Window::GetHandle() const {
    return m_window;
}

VkExtent2D Window::GetRenderExtent(Settings& settings) {

    m_renderExtent.width = settings.WIDTH;
    m_renderExtent.height = settings.HEIGHT;

    return m_renderExtent;

}

VkExtent2D Window::GetWindowExtent(Display& display, Settings& settings) {

    if (settings.FULLSCREEN) {
        m_windowExtent.width = static_cast<uint32_t>(display.GetCurrentDisplayMode()->w * display.GetScaling());
        m_windowExtent.height = static_cast<uint32_t>(display.GetCurrentDisplayMode()->h * display.GetScaling());
    } else {
        m_windowExtent.width = settings.WIDTH * settings.WINDOWED_SCALE;
        m_windowExtent.height = settings.HEIGHT * settings.WINDOWED_SCALE;
    }

    return m_windowExtent;

}


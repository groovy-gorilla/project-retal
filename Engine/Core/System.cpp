#include "pch.h"
#include "System.h"
#include <cpuid.h>


void System::Run() {

    setenv("GTK_THEME", "Adwaita:dark", true);

    // Sprawdzenie czy CPU obsługuje AVX dla Lina64
    if (!CheckAVXSupport()) {
        throw std::runtime_error("Your CPU does not support AVX instructions.");
    }

    // SDL Init
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(SDL_GetError());
    }

    // Display init
    m_display.Initialize();

    // Window init
    m_window.Create(m_desc, m_display);

    // Input init
    m_input.Initialize();
    m_sdlInput.Initialize(&m_input);

    // Graphics init
    m_graphics.Initialize(m_display, m_window, m_desc);

    // Main loop
    while (!m_window.ShouldClose()) {

        m_timer.Update();

        m_input.Update();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:                                   // kliknięcie X w oknie
                    //m_window.SetShouldClose(true);
                    break;
                case SDL_EVENT_QUIT:                                                     // zakończenie programu
                    m_window.SetShouldClose(true);
                    break;
                case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:                               // zmiana rozmiaru framebuffer
                    //m_framebufferResized = true;
                    break;
                case SDL_EVENT_WINDOW_RESIZED:                                          // zmiana rozmiaru okna
                    // ...
                    break;
                case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:                            // zmiana skali w systemie
                    // ...
                    break;
                case SDL_EVENT_WINDOW_MINIMIZED:                                        // minimalizacja okna (utrata fokusa i okno ma wtedy zwykle rozmiar 0x0
                    // Na Wayland nie działa (póki co)
                    break;
                case SDL_EVENT_WINDOW_RESTORED:                                         // przywrócenie okna z mimimalizacji
                    // Na Wayland nie działa (póki co)
                    break;
                case SDL_EVENT_WINDOW_MAXIMIZED:                                        // maksymalizacja okna
                    // ...
                    break;
                case SDL_EVENT_WINDOW_FOCUS_LOST:                                       // utrata fokusa --> robimy pauzę
                    m_paused = false;
                    //SDL_SetWindowRelativeMouseMode(m_window.GetHandle(), false);
                    break;
                case SDL_EVENT_WINDOW_FOCUS_GAINED:                                     // przywrócenie fokusa
                    m_paused = true;                                                    // ale co jeśli podczas utraty fokusa doszło np. do zmiany skali?
                    //SDL_SetWindowRelativeMouseMode(m_window.GetHandle(), true);
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:                                       // uchwycenie myszy po kliknięciu nią w okno
                    // SDL_SetRelativeMouseMode(SDL_TRUE);
                    break;
                case SDL_EVENT_MOUSE_MOTION:
                    m_input.SetMouseDelta(event.motion.xrel, event.motion.yrel);
                    break;
                case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:
                    // ...
                    break;
                case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
                    // ...
                    break;
                default:
                    // ...
                    break;
            }

            m_sdlInput.ProcessEvent(event);

        }

        // PAUSED
        if (!m_paused) {
            SDL_Delay(100);
            continue;
        }

        // RENDER
        m_graphics.Render(m_desc, m_timer.GetDeltaTime());

        // MOUSE ROTATE CAMERA
        double sensitivity = 0.002;
        m_graphics.GetCamera().AddRotation(
            -m_input.GetMouseDelta().y * sensitivity,
            -m_input.GetMouseDelta().x * sensitivity
        );

        // WSAD
        double moveSpeed = 50.0;
        double moveStep = moveSpeed * m_timer.GetDeltaTime();
        if (m_input.IsHeld(Action::Forward)) m_graphics.GetCamera().MoveForward(moveStep);
        if (m_input.IsHeld(Action::Backward)) m_graphics.GetCamera().MoveForward(-moveStep);
        if (m_input.IsHeld(Action::Right)) m_graphics.GetCamera().MoveRight(moveStep);
        if (m_input.IsHeld(Action::Left)) m_graphics.GetCamera().MoveRight(-moveStep);
        if (m_input.IsHeld(Action::Up)) m_graphics.GetCamera().MoveUp(moveStep);
        if (m_input.IsHeld(Action::Down)) m_graphics.GetCamera().MoveUp(-moveStep);

        // HDR ON/OFF
        if (m_input.IsPressed(Action::HDR)) {
            m_desc.HDR = !m_desc.HDR;
        }

        // HDR EXPOSURE
        static bool explosion = true;
        if (m_input.IsPressed(Action::Exposure)) {
            if (explosion) {
                m_graphics.GetRenderer().SetTargetExposure(5.0f);
                explosion = false;
            } else {
                m_graphics.GetRenderer().SetTargetExposure(1.0f);
                explosion = true;
            }
        }

        // DITHERING ON/OFF
        if (m_input.IsPressed(Action::Dithering)) {
            m_desc.DITHERING = !m_desc.DITHERING;
        }

        // ASPECT RATIO
        if (m_input.IsPressed(Action::Aspect)) {
            m_desc.ASPECT_RATIO = !m_desc.ASPECT_RATIO;
        }

        // FILTER
        if (m_input.IsPressed(Action::Filter)) {
            if (m_desc.FILTER == TextureFilter::Nearest) {
                m_desc.FILTER = TextureFilter::Linear;
            } else {
                m_desc.FILTER = TextureFilter::Nearest;
            }
        }

        // VSYNC
        if (m_input.IsPressed( Action::VSync)) {
            m_desc.VSYNC = !m_desc.VSYNC;
            m_graphics.GetRenderer().RecreateSwapchain(m_display, m_window, m_desc);
        }

        // WINDOWED
        if (m_input.IsPressed(Action::Windowed)) {
            if (m_desc.FULLSCREEN) {
                m_desc.FULLSCREEN = false;
                m_window.SetWindowed(m_desc, m_display);
            } else {
                m_desc.FULLSCREEN = true;
                m_window.SetFullscreen(m_desc, m_display);
            }

            m_graphics.GetRenderer().RecreateRenderer(m_display, m_window, m_desc);
        }

        // SCREEN RESOLUTION
        int size = m_display.GetDisplayModes().size();
        std::vector<Mode> m = m_display.GetDisplayModes();
        auto it = std::find_if(m.begin(), m.end(),
            [&](const Mode& r) {
                return r.width == m_desc.WIDTH && r.height == m_desc.HEIGHT;
            });

        static int i = -1;
        if (it != m_display.GetDisplayModes().end()) {
            i = std::distance(m.begin(), it);
        }

        if (m_input.IsPressed(Action::ResolutionDown)) {
            i++;
            if (i > (size-1)) i = (size-1);
            SetResolution(m[i]);
            std::cout << "Resolution set to: " << m[i].width << "x" << m[i].height << std::endl;
        }

        if (m_input.IsPressed(Action::ResolutionUp)) {
            i--;
            if (i < 0) i = 0;
            SetResolution(m[i]);
            std::cout << "Resolution set to: " << m[i].width << "x" << m[i].height << std::endl;
        }

        // ANTIALIASING
        if (m_input.IsPressed(Action::AntiAliasing)) {
            switch (m_desc.AA_MODE) {
                case AntiAliasing::None:
                    m_desc.AA_MODE = AntiAliasing::MSAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_16_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    std::cout << "AA_MODE: MSAA" << std::endl;
                    break;
                case AntiAliasing::MSAA:
                    m_desc.AA_MODE = AntiAliasing::SMAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    std::cout << "AA_MODE: SMAA" << std::endl;
                    break;
                case AntiAliasing::SMAA:
                    m_desc.AA_MODE = AntiAliasing::MSAA_SMAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_16_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    std::cout << "AA_MODE: MSAA_SMAA" << std::endl;
                    break;
                case AntiAliasing::MSAA_SMAA:
                    m_desc.AA_MODE = AntiAliasing::SSAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 2.0f;
                    std::cout << "AA_MODE: SSAA" << std::endl;
                    break;
                case AntiAliasing::SSAA:
                    m_desc.AA_MODE = AntiAliasing::SSAA_SMAA;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 2.0f;
                    std::cout << "AA_MODE: SSAA_SMAA" << std::endl;
                    break;
                case AntiAliasing::SSAA_SMAA:
                    m_desc.AA_MODE = AntiAliasing::None;
                    m_desc.MSAA_SAMPLES = VK_SAMPLE_COUNT_1_BIT;
                    m_desc.SSAA_SCALE = 1.0f;
                    std::cout << "AA_MODE: NONE" << std::endl;
                    break;
            }
            m_graphics.Recreate(m_display, m_window, m_desc);
        }

        // MOUSE CAPTURED
        if (m_input.IsPressed(Action::ToggleMouse)) {
            m_mouseCaptured = !m_mouseCaptured;

            SDL_SetWindowRelativeMouseMode(
                m_window.GetHandle(),
                m_mouseCaptured
            );
        }

        // TAKE SCREENSHOT
        if (m_input.IsPressed(Action::Screenshot)) {
            m_desc.TAKE_SCREENSHOT = true;
        }

        // QUIT
        if (m_input.IsPressed(Action::Quit)) break;

    }

    m_graphics.Shutdown();
    m_window.Destroy();
    SDL_Quit();

}

void System::SetResolution(const Mode& res) {

    // 1. Window desc
    m_desc.WIDTH = res.width;
    m_desc.HEIGHT = res.height;

    // 2. Zmienia rozmiar okna
    if (m_desc.FULLSCREEN) {
        // ...
    } else {
        m_window.SetWindowSize(m_desc, m_display);
    }

    // 3. Renderer ogarnia swapchain
    m_graphics.GetRenderer().RecreateRenderer(m_display, m_window, m_desc);

}

bool System::CheckAVXSupport() {

    unsigned int eax, ebx, ecx, edx;

    if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) return false;

    bool avxSupported = (ecx & bit_AVX) != 0;

    bool osxsaveSupported = (ecx & bit_OSXSAVE) != 0;

    if (!avxSupported || !osxsaveSupported) return false;

    uint64_t xcr0 = _xgetbv(0);

    return (xcr0 & 0x6) == 0x6;

}


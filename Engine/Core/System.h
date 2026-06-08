#pragma once

#include "Settings.h"
#include "Display.h"
#include "Timer.h"
#include "Window.h"
#include "Input/Input.h"
#include "Platform/SDL/SDLInput.h"
#include "Graphics/Graphics.h"

class Display;

class System {
public:
    void Run();

    void SetResolution(const Mode& res);

private:
    bool m_running = true;
    bool m_paused = true;

    Settings m_desc;
    Display m_display;
    Window m_window;
    Timer m_timer;
    Input m_input;
    SDLInput m_sdlInput;
    Graphics m_graphics;

    bool CheckAVXSupport();

};
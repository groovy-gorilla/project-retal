#pragma once
#include "Math/Lina64.h"

enum class Action {
    Quit,
    Windowed,
    Aspect,
    Filter,
    AntiAliasing,
    VSync,
    Screenshot,
    Exposure,
    HDR,
    Dithering,
    ResolutionUp,
    ResolutionDown,
    ToggleMouse,
    Forward,
    Backward,
    Left,
    Right,
    Up,
    Down
};

class Input {

public:
    void Initialize();
    void Update();

    // KEYBOARD
    void SetKeyState(SDL_Scancode scancode, bool pressed);
    bool IsPressed(Action action) const;
    bool IsHeld(Action action) const;
    bool IsReleased(Action action) const;

    // MOUSE
    lina::fvec2 GetMouseDelta() const;
    void SetMouseDelta(int deltaX, int deltaY);

private:
    std::unordered_map<Action, SDL_Scancode> m_bindings;
    std::vector<uint8_t> m_currentKeys;
    std::vector<uint8_t> m_previousKeys;

    int m_mouseDeltaX = 0;
    int m_mouseDeltaY = 0;

};

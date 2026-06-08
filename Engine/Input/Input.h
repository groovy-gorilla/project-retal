#pragma once

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
    ResolutionDown
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

private:
    std::unordered_map<Action, SDL_Scancode> m_bindings;
    std::vector<uint8_t> m_currentKeys;
    std::vector<uint8_t> m_previousKeys;

};

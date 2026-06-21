#include "pch.h"
#include "Input.h"

void Input::Initialize() {

    m_currentKeys.resize(SDL_SCANCODE_COUNT);
    m_previousKeys.resize(SDL_SCANCODE_COUNT);

    m_bindings[Action::Quit]            = SDL_SCANCODE_ESCAPE;
    m_bindings[Action::Windowed]        = SDL_SCANCODE_I;
    m_bindings[Action::Aspect]          = SDL_SCANCODE_T;
    m_bindings[Action::Filter]          = SDL_SCANCODE_F;
    m_bindings[Action::AntiAliasing]    = SDL_SCANCODE_M;
    m_bindings[Action::VSync]           = SDL_SCANCODE_V;
    m_bindings[Action::Screenshot]      = SDL_SCANCODE_P;
    m_bindings[Action::Exposure]        = SDL_SCANCODE_X;
    m_bindings[Action::HDR]             = SDL_SCANCODE_H;
    m_bindings[Action::Dithering]       = SDL_SCANCODE_C;
    m_bindings[Action::Vga]             = SDL_SCANCODE_K;
    m_bindings[Action::ResolutionUp]    = SDL_SCANCODE_EQUALS;
    m_bindings[Action::ResolutionDown]  = SDL_SCANCODE_MINUS;
    m_bindings[Action::ToggleMouse]     = SDL_SCANCODE_1;

    m_bindings[Action::Forward]         = SDL_SCANCODE_W;
    m_bindings[Action::Backward]        = SDL_SCANCODE_S;
    m_bindings[Action::Left]            = SDL_SCANCODE_A;
    m_bindings[Action::Right]           = SDL_SCANCODE_D;

    m_bindings[Action::Up]              = SDL_SCANCODE_Q;
    m_bindings[Action::Down]            = SDL_SCANCODE_E;

}

void Input::Update() {

    m_previousKeys = m_currentKeys;

    m_mouseDeltaX = 0;
    m_mouseDeltaY = 0;
}

void Input::SetKeyState(SDL_Scancode scancode, bool pressed) {
    m_currentKeys[scancode] = pressed;
}

bool Input::IsPressed(Action action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return false;
    return m_currentKeys[it->second] && !m_previousKeys[it->second];
}

bool Input::IsHeld(Action action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return false;
    return m_currentKeys[it->second];
}

bool Input::IsReleased(Action action) const {
    auto it = m_bindings.find(action);
    if (it == m_bindings.end()) return false;
    return !m_currentKeys[it->second] && m_previousKeys[it->second];
}

lina::fvec2 Input::GetMouseDelta() const {
    return lina::fvec2(m_mouseDeltaX, m_mouseDeltaY);
}


void Input::SetMouseDelta(int deltaX, int deltaY) {
    m_mouseDeltaX += deltaX;
    m_mouseDeltaY += deltaY;
}









#include "pch.h"
#include "SDLInput.h"

void SDLInput::Initialize(Input* input) {
    m_input = input;
}

void SDLInput::ProcessEvent(const SDL_Event& e) {
    if (e.type == SDL_EVENT_KEY_DOWN) {
        m_input->SetKeyState(e.key.scancode, true);
    }
    else if (e.type == SDL_EVENT_KEY_UP) {

        m_input->SetKeyState(e.key.scancode, false);
    }
}





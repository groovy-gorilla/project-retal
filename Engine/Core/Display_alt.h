#pragma once

struct Mode {
    int width;
    int height;
    std::string ratio;
};


class Display {
public:
    void Initialize();

    [[nodiscard]] const SDL_DisplayID GetCurrentDisplay() const;                     // Pobiera główny monitor

    [[nodiscard]] std::vector<Mode> GetDisplayModes() const;                        // Pobiera tryby monitora

    const SDL_DisplayMode* GetCurrentDisplayMode();                                 // Pobiera aktualny tryb monitora

    float GetScaling();                                                             // Pobiera skalowanie

private:
    SDL_DisplayID m_currentDisplay;
    const char* m_currentDisplayName;
    SDL_DisplayMode* m_currentMode = nullptr;
    std::vector<Mode> m_videoModes;

};

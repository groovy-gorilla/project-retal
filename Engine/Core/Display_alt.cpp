#include "pch.h"
#include "Display.h"


void Display::Initialize() {

    SDL_DisplayID* displays = nullptr;

    // Pobiera listę monitorów
    displays = SDL_GetDisplays(nullptr);
    if (!displays) {
        throw std::runtime_error(SDL_GetError());
    }

    // Tworzy listę informacji o monitorze
    m_currentDisplay = displays[0];                                                 // Wybiera główny monitor (SDL_GetPrimaryDisplay często nie działa prawidłowo)
    m_currentDisplayName = SDL_GetDisplayName(displays[0]);                         // Pobiera nazwę monitora

    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displays[0], nullptr);    // Pobiera tryby monitora

    m_currentMode = modes[0];                                                               // Ustawia tryb aktualny

    // ******************************************************************************************************************************

    const float epsilon = 0.07f;

    std::vector<int> heightModes;

    // Szukamy najmniejszego dzielnika bez części ułamkowej powyżej 100px
    int lastValidHeight = m_currentMode->h;
    for (int divisor = 2;; ++divisor) {
        if (m_currentMode->h % divisor != 0) continue;
        int height = m_currentMode->h / divisor;
        if (height < 100) break;
        lastValidHeight = height;
    }

    // Uzupełniamy tablicę wysokości o kolejne dzielniki
    int multipleHeight = lastValidHeight;
    while (multipleHeight <= m_currentMode->h) {
        heightModes.push_back(multipleHeight);
        multipleHeight += lastValidHeight;
    }

    // Aspect natywny
    float nativeAspect = static_cast<float>(m_currentMode->w) / static_cast<float>(m_currentMode->h);

    // Uzupełniamy powyższą tablicę trybów
    // Dodajemy AKTUALNĄ
    for (int i = 0; i < heightModes.size(); i++) {
        Mode m;
        m.height = heightModes[i];
        m.width = std::round(m.height * nativeAspect);
        m_videoModes.push_back(m);
    }

    // I kolejne:
    float targetAspect = 0.0f;

    // 32:9
    targetAspect = 32.0f / 9.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 32.0f / 9.0f);
            m_videoModes.push_back(m);
        }
    }

    // 21:9
    targetAspect = 21.0f / 9.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 21.0f / 9.0f);
            m_videoModes.push_back(m);
        }
    }

    // 16:9
    targetAspect = 16.0f / 9.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 16.0f / 9.0f);
            m_videoModes.push_back(m);
        }
    }

    // 16:10
    targetAspect = 16.0f / 10.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 16.0f / 10.0f);
            m_videoModes.push_back(m);
        }
    }

    // 3:2
    targetAspect = 3.0f / 2.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 3.0f / 2.0f);
            m_videoModes.push_back(m);
        }
    }

    // 4:3
    targetAspect = 4.0f / 3.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 4.0f / 3.0f);
            m_videoModes.push_back(m);
        }
    }

    // 5:4
    targetAspect = 5.0f / 4.0f;
    if (std::abs(nativeAspect - targetAspect) > epsilon) {
        for (int i = 0; i < heightModes.size(); i++) {
            Mode m;
            m.height = heightModes[i];
            m.width = std::round(static_cast<float>(heightModes[i]) * 5.0f / 4.0f);
            m_videoModes.push_back(m);
        }
    }

    // Eliminujemy te, które aspect szerokości jest większy od natywnej
    for (auto it = m_videoModes.begin(); it != m_videoModes.end();) {

        float rat = static_cast<float>(it->width) / static_cast<float>(it->height);

        if (rat > nativeAspect + epsilon) {
            it = m_videoModes.erase(it);
        } else {
            ++it;
        }
    }

    // Eliminujemy te, których height jest mniejsze od 200px
    for (auto it = m_videoModes.begin(); it != m_videoModes.end();) {

        if (it->height < 200) {
            it = m_videoModes.erase(it);
        } else {
            ++it;
        }

    }

    // Przygotowujemy tablicę aspectów
    struct R { float value; std::string name; };
    R known[] = {
        {32.0f/9.0f, "(32:9)"},
        {21.0f/9.0f, "(21:9)"},
        {16.0f/9.0f, "(16:9)"},
        {16.0f/10.0f, "(16:10)"},
        {3.0f/2.0f, "(3:2)"},
        {4.0f/3.0f, "(4:3)"},
        {5.0f/4.0f, "(5:4)"}
    };


    // Wyliczamy dla każdego trybu współczynnik ratio
    for (auto& mode : m_videoModes) {
        float rat = static_cast<float>(mode.width) / mode.height;

        float bestDiff = std::numeric_limits<float>::max();
        std::string best = "custom";

        for (auto& k : known) {
            float diff = std::abs(rat - k.value);
            if (diff < bestDiff) {
                bestDiff = diff;
                best = k.name;
            }
        }

        mode.ratio = best;

    }

    // Sortujemy listę od największej
    std::sort(m_videoModes.begin(),
              m_videoModes.end(),
              [](const Mode& a, const Mode& b) {
                  if (a.height != b.height) return a.height > b.height;
                  return a.width > b.width;
              });


    for (int i = 0; i < m_videoModes.size(); i++) {
        std::cout << m_videoModes[i].width << "x" << m_videoModes[i].height << " " << m_videoModes[i].ratio << std::endl;
    }

    SDL_free(displays);

}

const SDL_DisplayID Display::GetCurrentDisplay() const {
    return m_currentDisplay;
}

std::vector<Mode> Display::GetDisplayModes() const {
    return m_videoModes;
}

const SDL_DisplayMode* Display::GetCurrentDisplayMode() {
    return SDL_GetCurrentDisplayMode(m_currentDisplay);
}

float Display::GetScaling() {
    const SDL_DisplayMode* mcurr = SDL_GetCurrentDisplayMode(m_currentDisplay);
    if (!mcurr) { return 1.0f; }
    float scaling = static_cast<float>(m_currentMode->w) / mcurr->w;
    std::cout << "Scaling: " << scaling << std::endl;
    return scaling;
}
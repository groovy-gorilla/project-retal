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

    int count = 0;
    SDL_DisplayMode** modes = SDL_GetFullscreenDisplayModes(displays[0], &count);         // Pobiera tryby monitora i ich ilość
    m_videoModes.resize(count);
    for (int i = 0; i < m_videoModes.size(); i++) {
        m_videoModes[i].width = modes[i]->w;
        m_videoModes[i].height = modes[i]->h;
    }

    m_currentMode = modes[0];                                                               // Ustawia tryb aktualny

    // ******************************************************************************************************************************

    // Wstępna lista trybów domyślnych
    std::vector<Mode> defaultModes = {
        {320, 200, "(16:10)"},
        {320, 240, "(4:3)"},
        {640, 360, "(16:9)"},
        {640, 480, "(4:3)"},
        {720, 480, "(3:2)"},
        {800, 600, "(4:3)"},
        {854, 480, "(16:9)"},
        {960, 540, "(16:9)"},     // qHD
        {960, 640, "(3:2)"},
        {1024, 576, "(16:9)"},
        {1024, 768, "(4:3)"},
        {1152, 864, "(4:3)"},
        {1280, 720, "(16:9)"},    // HD
        {1280, 800, "(16:10)"},
        {1280, 960, "(4:3)"},
        {1280, 1024, "(5:4)"},
        {1360, 768, "(16:9)"},
        {1366, 768, "(16:9)"},
        {1400, 1050, "(4:3)"},
        {1440, 900, "(16:10)"},
        {1440, 960, "(3:2)"},
        {1536, 864, "(16:9)"},
        {1600, 900, "(16:9)"},
        {1600, 1200, "(4:3)"},
        {1680, 1050, "(16:10)"},
        {1920, 1080, "(16:9)"},   // FullHD
        {1920, 1200, "(16:10)"},
        {2048, 1152, "(16:9)"},
        {2048, 1536, "(4:3)"},
        {2160, 1440, "(3:2)"},
        {2240, 1400, "(16:10)"},
        {2256, 1504, "(3:2)"},
        {2560, 1080, "(21:9)"},
        {2560, 1440, "(16:9)"},   // 2K QHD
        {2560, 1600, "(16:10)"},
        {2560, 1920, "(4:3)"},
        {2560, 2048, "(5:4)"},
        {2736, 1824, "(3:2)"},
        {2880, 1800, "(16:10)"},
        {2880, 1920, "(3:2)"},
        {3000, 2000, "(3:2)"},
        {3200, 1800, "(16:9)"},   // 3K
        {3240, 2160, "(3:2)"},
        {3440, 1440, "(21:9)"},
        {3840, 1080, "(32:9)"},
        {3840, 1600, "(21:9)"},
        {3840, 2160, "(16:9)"},   // 4K UHD
        {3840, 2400, "(16:10)"},
        {4500, 3000, "(3:2)"},
        {5120, 1440, "(32:9)"},
        {5120, 2160, "(21:9)"},
        {5120, 2880, "(16:9)"},   // 5K
        {6016, 3384, "(16:9)"},
        {6144, 3456, "(16:9)"},   // 6K
        {7680, 2160, "(32:9)"},
        {7680, 4320, "(16:9)"}    // 8K
    };

    // Tworzymy kopię listy trybów monitora
    std::vector<Mode> modeCopy = m_videoModes;

    // Wrzucamy listę trybów domyślnych do listy głównej trybów
    for (const auto& dMode : defaultModes) {

        bool found = false;

        // wyszukujemy powtórzenia
        for (const auto& cMode : modeCopy) {
            if (cMode.width == dMode.width && cMode.height == dMode.height) {
                found = true;
                break;
            }
        }

        // jeśli nie ma to wpisujemy do listy
        if (!found) {
            if (dMode.width < modeCopy[0].width && dMode.height < modeCopy[0].height) {     // pomija większe niż natywna
                m_videoModes.push_back(dMode);
            }
        }

    }

    // Lista główna trybów z początku zawiera tryby z monitora.
    // Następnie do tej listy dorzucane są tryby z listy domyślnej.
    // Sprawdza czy dany tryb już jest na liście, jeśli tak to pomija.


    // Przygotowujemy tablicę aspectów
    struct R {
        float value;
        std::string name;
    };

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
    constexpr float epsilon = 0.07f;

    for (auto& mode : m_videoModes) {

        float rat = static_cast<float>(mode.width) / static_cast<float>(mode.height);

        mode.ratio = "(custom)";

        for (const auto& k : known) {
            float diff = std::abs(rat - k.value);
            if (diff <= epsilon) {
                mode.ratio = k.name;
                break;
            }
        }

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
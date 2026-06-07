#pragma once
#include <cstdint>

class Fps {
public:
    void Update(float deltaTime);

    [[nodiscard]] uint32_t GetFPS() const { return m_fps; }

private:
    float m_accumulator = 0.0f;
    uint32_t m_frameCount = 0;
    uint32_t m_fps = 0;
};

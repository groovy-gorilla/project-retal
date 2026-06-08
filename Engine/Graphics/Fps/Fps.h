#pragma once

class Fps {
public:
    void Update(float deltaTime);

    [[nodiscard]] uint32_t GetFPS() const;
    [[nodiscard]] float GetFrameTime() const;

private:
    float m_accumulator = 0.0f;
    float m_frameTime = 0.0f;
    float m_frameTimeOut = 0.0f;
};

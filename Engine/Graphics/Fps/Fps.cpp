#include "Fps.h"

void Fps::Update(float deltaTime) {

    m_frameTime = m_frameTime * 0.90f + deltaTime * 1000.0f * 0.1f;

    m_accumulator += deltaTime;

    if (m_accumulator >= 0.25f) {

        m_frameTimeOut = m_frameTime;

        m_accumulator -= 0.25f;

    }

}

uint32_t Fps::GetFPS() const {
    if (m_frameTimeOut <= 0.0f)return 0;
    return static_cast<uint32_t>(std::round(1000.0f / m_frameTimeOut));
}

float Fps::GetFrameTime() const {
    return m_frameTimeOut;
}
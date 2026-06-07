#include "Fps.h"

void Fps::Update(float deltaTime) {

    m_accumulator += deltaTime;
    m_frameCount++;

    if (m_accumulator >= 0.25f) {

        m_fps = m_frameCount * 4;

        m_frameCount = 0;
        m_accumulator -= 0.25f;

    }

}

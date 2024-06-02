/*
 * BlinkingAnimation.h
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#ifndef GUI_INCLUDE_GUI_COMMON_BLINKINGANIMATION_H_
#define GUI_INCLUDE_GUI_COMMON_BLINKINGANIMATION_H_

#include <utils/timeout.h>

template <typename Target>
class BlinkingAnimation {
public:
    BlinkingAnimation(Target& target, uint16_t duration) : m_target(target), m_duration(duration) {}

    void handleTickEvent()
    {
        static const int16_t frameRate = 1000/60;
        static const uint8_t maxAlpha = 255;
        int duration = HAL_GetTick() - m_dirChangeTick;
        auto frame = duration / frameRate;
        if (frame != m_lastFrame)
        {
            if (duration >= m_duration) {
                m_dirChangeTick = HAL_GetTick();
                m_target.setAlpha(maxAlpha);
            } else {
                // ((m_duration - duration) * (-255) + duration * 255)/ m_duration
                auto nAlpha = 2*maxAlpha*duration/m_duration - maxAlpha;
                m_target.setAlpha(nAlpha < 0 ? -nAlpha : nAlpha);
                m_lastFrame = frame;
            }

            m_target.invalidate();
        }
    }
protected:
    Target& m_target;
    uint32_t m_dirChangeTick{0};
    uint16_t m_duration{1000};
    uint8_t m_lastFrame{0};
};

#endif /* GUI_INCLUDE_GUI_COMMON_BLINKINGANIMATION_H_ */

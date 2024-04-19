#ifndef ANIMATEDWARNING_HPP
#define ANIMATEDWARNING_HPP

#include <gui_generated/containers/AnimatedWarningBase.hpp>

class AnimatedWarning : public AnimatedWarningBase
{
public:
    AnimatedWarning();
    virtual ~AnimatedWarning();

    virtual void initialize();
    virtual void handleTickEvent();
protected:
    // animation
    uint32_t m_dirChangeTick{0};
    uint8_t m_lastFrame{0};
};

#endif // ANIMATEDWARNING_HPP

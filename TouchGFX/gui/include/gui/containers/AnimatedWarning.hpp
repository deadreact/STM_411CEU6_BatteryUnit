#ifndef ANIMATEDWARNING_HPP
#define ANIMATEDWARNING_HPP

#include <gui_generated/containers/AnimatedWarningBase.hpp>
#include <gui/common/BlinkingAnimation.h>

class AnimatedWarning : public AnimatedWarningBase
{
public:
    AnimatedWarning();
    virtual ~AnimatedWarning();

    virtual void initialize();
    virtual void handleTickEvent();
protected:
    BlinkingAnimation<touchgfx::Image> m_animation{image, 800};
};

#endif // ANIMATEDWARNING_HPP

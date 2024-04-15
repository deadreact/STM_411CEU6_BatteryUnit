#ifndef ANIMATEDWARNING_HPP
#define ANIMATEDWARNING_HPP

#include <gui_generated/containers/AnimatedWarningBase.hpp>

class AnimatedWarning : public AnimatedWarningBase
{
public:
    AnimatedWarning();
    virtual ~AnimatedWarning() {}

    virtual void initialize();
    virtual void handleTickEvent();
protected:
    int m_dir{-5};
};

#endif // ANIMATEDWARNING_HPP

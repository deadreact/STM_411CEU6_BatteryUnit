#include <gui/containers/AnimatedWarning.hpp>

AnimatedWarning::AnimatedWarning()
{
	touchgfx::Application::getInstance()->registerTimerWidget(this);
}

AnimatedWarning::~AnimatedWarning()
{
	touchgfx::Application::getInstance()->unregisterTimerWidget(this);
}

void AnimatedWarning::initialize()
{
    AnimatedWarningBase::initialize();
}

void AnimatedWarning::handleTickEvent()
{
	if (isVisible()) {
		m_animation.handleTickEvent();
	}
}

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
	if (isVisible())
	{
		int alpha = image.getAlpha() + m_dir;
		if (alpha <= 0 || alpha >= 255)
		{
			m_dir = -m_dir;
		}
		image.setAlpha(alpha);
		image.invalidateContent();
	}
}

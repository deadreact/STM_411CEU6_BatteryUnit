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
	static const int16_t animDuration = 800;
	static const int16_t frameRate = 1000/60;
	if (isVisible())
	{
		int duration = HAL_GetTick() - m_dirChangeTick;
		auto frame = duration / frameRate;
		if (frame != m_lastFrame)
		{
			if (duration >= animDuration) {
				m_dirChangeTick = HAL_GetTick();
				image.setAlpha(255);
			} else {
				// ((animDuration - duration) * (-255) + duration * 255)/ animDuration
				auto nAlpha = 510*duration/animDuration - 255;
				image.setAlpha(nAlpha < 0 ? -nAlpha : nAlpha);
				m_lastFrame = frame;
			}

			image.invalidate();
		}
	}
}

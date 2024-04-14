#include <gui/containers/AnimatedWarning.hpp>

AnimatedWarning::AnimatedWarning()
{

}

void AnimatedWarning::initialize()
{
    AnimatedWarningBase::initialize();
}

void AnimatedWarning::handleTickEvent()
{
	if (isVisible())
	{
//		auto dt = HAL_GetTick() - m_tick;
		if (true)
		{
			auto alpha = image.getAlpha() + m_dir;
			if (alpha == 0 || alpha == 255)
			{
				m_dir = -m_dir;
			}
			image.setAlpha(alpha);
			image.invalidateContent();
//			m_tick = HAL_GetTick();
		}
	}
}

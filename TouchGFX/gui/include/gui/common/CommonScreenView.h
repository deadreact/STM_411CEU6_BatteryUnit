/*
 * CommonScreenView.h
 *
 *  Created on: Oct 06, 2024
 *      Author: deadreact
 */

#ifndef GUI_INCLUDE_GUI_COMMON_COMMONSCREENVIEW_H_
#define GUI_INCLUDE_GUI_COMMON_COMMONSCREENVIEW_H_

#include <type_traits>
#include <touchgfx/Screen.hpp>
#include <gui/containers/Settings.hpp>

//class Settings;

template <class View>
class CommonScreenView : public View
{
	static_assert(std::is_base_of_v<touchgfx::Screen, View>);
public:
	void setSettingsPopUp(Settings* settings)
	{
		if (m_settingsPopUp != settings)
		{
			if (m_settingsPopUp)
			{
				View::remove(*m_settingsPopUp);
			}
			if (settings)
			{
				View::add(*settings);
			}
			m_settingsPopUp = settings;
			View::invalidate();
		}
	}
private:
	Settings* m_settingsPopUp{nullptr};
};

#endif //GUI_INCLUDE_GUI_COMMON_COMMONSCREENVIEW_H_

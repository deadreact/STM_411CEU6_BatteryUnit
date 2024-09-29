#ifndef CLOCKSCREENVIEW_HPP
#define CLOCKSCREENVIEW_HPP

#include <gui_generated/clockscreen_screen/ClockScreenViewBase.hpp>
#include <gui/clockscreen_screen/ClockScreenPresenter.hpp>

class Settings;

class ClockScreenView : public ClockScreenViewBase
{
public:
    ClockScreenView();
    virtual ~ClockScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

    Settings* takeSettings();
	void setSettings(Settings* settings);

    void updateTime();
protected:
    Timeout m_timer{500};
    Settings* m_settingsPopUp{nullptr};
};

#endif // CLOCKSCREENVIEW_HPP

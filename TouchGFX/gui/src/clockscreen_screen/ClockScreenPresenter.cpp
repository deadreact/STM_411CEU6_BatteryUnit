#include <gui/clockscreen_screen/ClockScreenView.hpp>
#include <gui/clockscreen_screen/ClockScreenPresenter.hpp>

ClockScreenPresenter::ClockScreenPresenter(ClockScreenView& v)
    : view(v)
{

}

void ClockScreenPresenter::activate()
{
	view.setSettings(model->settingsPopUp);
}

void ClockScreenPresenter::deactivate()
{
	view.takeSettings();
}

void ClockScreenPresenter::onSettingsCreated(Settings* settings)
{
	view.setSettings(settings);
}

void ClockScreenPresenter::onSettingsWillBeDestroyed()
{
	view.takeSettings();
}

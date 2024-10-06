#include <gui/clockscreen_screen/ClockScreenView.hpp>
#include <gui/clockscreen_screen/ClockScreenPresenter.hpp>

ClockScreenPresenter::ClockScreenPresenter(ClockScreenView& v)
    : view(v)
{

}

void ClockScreenPresenter::activate()
{
	view.setSettingsPopUp(model->settingsPopUp);
}

void ClockScreenPresenter::deactivate()
{
	view.setSettingsPopUp(nullptr);
}

void ClockScreenPresenter::onSettingsCreated()
{
	view.setSettingsPopUp(model->settingsPopUp);
}

void ClockScreenPresenter::onSettingsWillBeDestroyed()
{
	view.setSettingsPopUp(nullptr);
}

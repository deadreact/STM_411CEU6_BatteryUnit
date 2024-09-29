#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>

DebugScreenPresenter::DebugScreenPresenter(DebugScreenView& v)
    : view(v)
{

}

void DebugScreenPresenter::activate()
{
	view.setSettings(model->settingsPopUp);
}

void DebugScreenPresenter::deactivate()
{
	view.takeSettings();
}

void DebugScreenPresenter::onSettingsCreated(Settings* settings)
{
	view.setSettings(settings);
}

void DebugScreenPresenter::onSettingsWillBeDestroyed()
{
	view.takeSettings();
}

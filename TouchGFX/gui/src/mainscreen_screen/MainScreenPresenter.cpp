#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{
	view.setSettings(model->settingsPopUp);
}

void MainScreenPresenter::deactivate()
{
	view.takeSettings();
}

void MainScreenPresenter::onSettingsCreated(Settings* settings)
{
	view.setSettings(settings);
}

void MainScreenPresenter::onSettingsWillBeDestroyed()
{
	view.takeSettings();
}

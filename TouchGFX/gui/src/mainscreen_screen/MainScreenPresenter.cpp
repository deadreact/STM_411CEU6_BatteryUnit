#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{
	view.setSettingsPopUp(model->settingsPopUp);
}

void MainScreenPresenter::deactivate()
{
	view.setSettingsPopUp(nullptr);
}

void MainScreenPresenter::onSettingsCreated()
{
	view.setSettingsPopUp(model->settingsPopUp);
}

void MainScreenPresenter::onSettingsWillBeDestroyed()
{
	view.setSettingsPopUp(nullptr);
}

#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{
	view.giveSettings(model->settingsPopUp);
	model->settingsPopUp = nullptr;
}

void MainScreenPresenter::deactivate()
{
	model->settingsPopUp = view.takeSettings();
}

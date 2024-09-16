#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>

DebugScreenPresenter::DebugScreenPresenter(DebugScreenView& v)
    : view(v)
{

}

void DebugScreenPresenter::activate()
{
	view.giveSettings(model->settingsPopUp);
	model->settingsPopUp = nullptr;
}

void DebugScreenPresenter::deactivate()
{
	model->settingsPopUp = view.takeSettings();
}

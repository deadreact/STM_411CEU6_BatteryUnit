#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>

DebugScreenPresenter::DebugScreenPresenter(DebugScreenView& v)
    : view(v)
{

}

void DebugScreenPresenter::activate()
{
	view.setSettingsPopUp(model->settingsPopUp);
}

void DebugScreenPresenter::deactivate()
{
	view.setSettingsPopUp(nullptr);
}

void DebugScreenPresenter::onSettingsCreated()
{
	view.setSettingsPopUp(model->settingsPopUp);
}

void DebugScreenPresenter::onSettingsWillBeDestroyed()
{
	view.setSettingsPopUp(nullptr);
}

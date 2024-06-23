#include <gui/common/FrontendApplication.hpp>
#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <gui/idlescreen_screen/IdleScreenView.hpp>
#include <gui/idlescreen_screen/IdleScreenPresenter.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <gui/common/FrontendHeap.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{

}


void FrontendApplication::handleTickEvent()
{
    model.tick();
    FrontendApplicationBase::handleTickEvent();

    showScreen(SharedData::getData().screenId);
}

void FrontendApplication::showScreen(ScreenId id)
{
	if (m_screenId != id)
	{
		m_screenId = id;
		switch (id)
		{
		case ScreenId::DebugScreen:
			touchgfx::makeTransition<IdleScreenView, IdleScreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		case ScreenId::MainScreen:
			touchgfx::makeTransition<Screen1View, Screen1Presenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		case ScreenId::ClockScreen:
			touchgfx::makeTransition<Screen2View, Screen2Presenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		default:
			break;
		}
	}
}



#include <gui/common/FrontendApplication.hpp>
#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>
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
			touchgfx::makeTransition<DebugScreenView, DebugScreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		case ScreenId::MainScreen:
			touchgfx::makeTransition<MainScreenView, MainScreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
//		case ScreenId::ClockScreen:
//			touchgfx::makeTransition<ClockScreenView, ClockScreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
//			break;
		default:
			break;
		}
	}
}



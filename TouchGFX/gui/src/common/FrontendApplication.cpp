#include <gui/common/FrontendApplication.hpp>
#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <gui/screen2_screen/Screen2View.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <touchgfx/transitions/NoTransition.hpp>
#include <gui/common/FrontendHeap.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
	, m_processId(SharedData::getProcessId())
{

}


void FrontendApplication::handleTickEvent()
{
	model.tick();
	FrontendApplicationBase::handleTickEvent();

	if (SharedData::getProcessId() != m_processId)
	{
		m_processId = SharedData::getProcessId();
		switch (m_processId)
		{
		case ProcessId::Startup:
			touchgfx::makeTransition<Screen1View, Screen1Presenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		case ProcessId::Idle:
			touchgfx::makeTransition<Screen2View, Screen2Presenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		default:
			break;
		}
	}
}

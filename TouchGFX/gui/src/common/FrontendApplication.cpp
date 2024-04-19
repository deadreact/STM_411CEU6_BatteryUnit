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
        {
        	showScreen(SharedData::getData<ProcessId::Idle>().screenId);
        }
            break;
        default:
            break;
        }
    }
    else if (m_processId == ProcessId::Idle)
    {
    	showScreen(SharedData::getData<ProcessId::Idle>().screenId);
    }
}

void FrontendApplication::showScreen(int id)
{
	if (m_screenId != id)
	{
		m_screenId = id;
		switch (id)
		{
		case 0:
			touchgfx::makeTransition<IdleScreenView, IdleScreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		case 1:
			touchgfx::makeTransition<Screen1View, Screen1Presenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		case 2:
			touchgfx::makeTransition<Screen2View, Screen2Presenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
			break;
		}
	}
}



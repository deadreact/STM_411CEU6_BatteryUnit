#include <gui/screen1_screen/Screen1View.hpp>
#include <shared_data.h>


Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{
	if (SharedData::getProcessId() == ProcessId::Startup)
	{
		auto value = SharedData::getData<ProcessId::Startup>().timeLeftToStandby/70;
		circleProgress1.setValue(value);
	}
}

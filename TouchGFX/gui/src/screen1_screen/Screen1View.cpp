#include <gui/screen1_screen/Screen1View.hpp>

extern int gTimeLeftToSleep;

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
    auto value = gTimeLeftToSleep/120;
    circleProgress1.setValue(value);
}

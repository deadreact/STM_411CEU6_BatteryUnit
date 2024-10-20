#include <gui/screen1_screen/Screen1View.hpp>

Screen1View::Screen1View()
{

}


void Screen1View::setTime(int64_t time)
{
	auto m = time / 60;
	auto h = m / 60;
	digitalClock1.setTime24Hour(h, m % 60, time % 60);
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

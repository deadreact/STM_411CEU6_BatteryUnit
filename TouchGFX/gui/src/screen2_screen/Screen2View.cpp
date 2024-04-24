#include <gui/screen2_screen/Screen2View.hpp>
#include <touchgfx/Color.hpp>

Screen2View::Screen2View()
{

}

void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

void Screen2View::handleTickEvent()
{
    updateTime();
}

Timeout showAlarmTimeout;
extern RTC_HandleTypeDef hrtc;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *phrtc)
{
	showAlarmTimeout.reset(5500);
}

void Screen2View::updateTime()
{
	if (m_timer.isReached())
	{
		RTC_DateTypeDef gDate;
		RTC_TimeTypeDef gTime;

		HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

		m_timer.reset(gTime.SubSeconds * 1000 / (gTime.SecondFraction + 1) + 10);

		time.setTime24Hour(gTime.Hours, gTime.Minutes, gTime.Seconds);

		Unicode::snprintf(dateDayBuffer, DATEDAY_SIZE, "%02d", gDate.Date);
		Unicode::snprintf(dateBuffer1, DATEBUFFER1_SIZE, "%02d", gDate.Month);
		Unicode::snprintf(dateBuffer2, DATEBUFFER2_SIZE, "%04d", 2000 + gDate.Year);

		dateDay.invalidate();
		date.invalidate();

		if (showAlarmTimeout.isReached())
		{
			alarm.setVisible(false);
			alarm.invalidate();
			showAlarmTimeout.invalidate();
		}
		else if (showAlarmTimeout.isValid())
		{
			if (!alarm.isVisible())
			{
				alarm.setVisible(true);
				alarm.invalidate();
			}
		}
	}
}



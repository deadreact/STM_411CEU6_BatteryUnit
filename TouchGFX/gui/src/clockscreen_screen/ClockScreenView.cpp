#include <gui/clockscreen_screen/ClockScreenView.hpp>
#include <touchgfx/Color.hpp>

ClockScreenView::ClockScreenView()
{

}

void ClockScreenView::setupScreen()
{
    ClockScreenViewBase::setupScreen();
}

void ClockScreenView::tearDownScreen()
{
    ClockScreenViewBase::tearDownScreen();
}

void ClockScreenView::handleTickEvent()
{
    updateTime();
}

Timeout showAlarmTimeout;
extern RTC_HandleTypeDef hrtc;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *phrtc)
{
	showAlarmTimeout.reset(5500);
}

void ClockScreenView::updateTime()
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



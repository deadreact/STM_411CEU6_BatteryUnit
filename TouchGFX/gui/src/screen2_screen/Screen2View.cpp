#include <gui/screen2_screen/Screen2View.hpp>
#include <touchgfx/Color.hpp>

uint32_t tickToShowAlarm = 0;

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
    if (SharedData::getProcessId() == ProcessId::Idle)
    {
        const auto& data = SharedData::getData<ProcessId::Idle>();
//        if (sleepingLabel.isVisible() != data.sleepingMode)
//        {
//            sleepingLabel.setVisible(data.sleepingMode);
//            sleepingLabel.invalidate();
//            container1.setVisible(!data.sleepingMode);
//            container1.invalidate();
//        }
//
//        if (data.sleepingMode) {
//            return;
//        }

        if (data.screenBrightness != m_screenBrightness)
        {
            static const float k = 1000.f/255;
            brightnessValue.setValue(data.screenBrightness/10);
            brightnessValue.setColor(touchgfx::Color::getColorFromRGB((1000 - data.screenBrightness)/k, data.screenBrightness/k, 0));
            m_screenBrightness = data.screenBrightness;
        }

        if (data.analog1 != m_analog1 || data.analog2 != m_analog2)
        {
            m_analog1 = data.analog1;
            m_analog2 = data.analog2;
            lineProgress1.setValue(m_analog1);
            lineProgress2.setValue(m_analog2);
        }
    }


    updateTime();
}

extern RTC_HandleTypeDef hrtc;

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *phrtc)
{
	tickToShowAlarm = HAL_GetTick() + 3300;
}

void Screen2View::updateTime()
{
	const auto currentTick = HAL_GetTick();
	if (m_lastUpdateTimeTick + 500 > currentTick) {
		return;
	}
	m_lastUpdateTimeTick = currentTick;

	RTC_DateTypeDef gDate;
	RTC_TimeTypeDef gTime;

	HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);

	time.setTime24Hour(gTime.Hours, gTime.Minutes, gTime.Seconds);

	Unicode::snprintf(dateDayBuffer, DATEDAY_SIZE, "%02d", gDate.Date);
	Unicode::snprintf(dateBuffer1, DATEBUFFER1_SIZE, "%02d", gDate.Month);
	Unicode::snprintf(dateBuffer2, DATEBUFFER2_SIZE, "%04d", 2000 + gDate.Year);

	dateDay.invalidate();
	date.invalidate();

	if (tickToShowAlarm > currentTick)
	{
		if (!alarm.isVisible())
		{
			alarm.setVisible(true);
			alarm.invalidate();
		}
	}
	else if (tickToShowAlarm != 0)
	{
		alarm.setVisible(false);
		alarm.invalidate();
		tickToShowAlarm = 0;
	}
}



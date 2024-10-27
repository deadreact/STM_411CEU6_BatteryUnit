/*
 * tft_display_320x240.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <ili9341.h>
#include <cmsis_os.h>
#include <handlers/display_handler.h>
#include <shared_data.h>
#include <cstring>

extern "C" {
extern osThreadId_t GUI_TaskHandle;
}

void Display320x240::onTick()
{
	constexpr static const uint32_t kStep = 10;

	if (!m_isActive) {
		return;
	}

	if (TIM2->CCR3 != m_brightness)
	{
		if (TIM2->CCR3 < m_brightness) {
			TIM2->CCR3 += kStep;
			if (TIM2->CCR3 > m_brightness) {
				TIM2->CCR3 = m_brightness;
			}
		}
		else if (TIM2->CCR3 - m_brightness > kStep) {
			TIM2->CCR3 -= kStep;
		} else {
			TIM2->CCR3 = m_brightness;
		}
	}

	const auto& data = *SharedData::getData();

	int brightnessSetting = data.settings.active ? data.settings.getValue(SettingsData::Brightness) : m_sensData.brightnessSetting;
	DisplaySensitiveData newData{data.screenId, data.invState, data.usbState, data.chargerPlugged, brightnessSetting};
	if (data.getPowerModeState() == PowerModeState::Normal && (m_sensData != newData || data.settings.active))
	{
		m_sensData = newData;
		on();
	}
	else if (m_brightnessTimeout.isReached())
	{
		if (isOn()) {
			m_brightness = (kOnRelaxed * m_sensData.brightnessSetting) / 100;
			m_brightnessTimeout.reset();
		} else {
			off();
		}
	}

	if (isOff())
	{
		if (m_guiActive)
		{
			m_guiActive = false;
			osThreadSuspend(GUI_TaskHandle);
		}
	}
	else if (!m_guiActive)
	{
		m_guiActive = true;
		osThreadResume(GUI_TaskHandle);
	}

	if (!isOff())
	{
//		MX_TouchGFX_Process();
	}
}

void Display320x240::on()
{
	if (isOff()) {
//		MX_TouchGFX_Process();
	}
	m_brightness = (kOn * m_sensData.brightnessSetting) / 100;
	m_brightnessTimeout.reset();
}

void Display320x240::off()
{
	m_brightness = (kOff * m_sensData.brightnessSetting) / 100;
}


void Display320x240::setActive(bool active)
{
	m_isActive = active;
	m_brightnessTimeout.setPaused(!active);
}

void Display320x240::toggle()
{
	if (isOn()) {
		off();
	} else {
		on();
	}
}

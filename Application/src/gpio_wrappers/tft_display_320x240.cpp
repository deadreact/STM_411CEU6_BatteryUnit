/*
 * tft_display_320x240.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/tft_display_320x240.h>
#include <ili9341.h>
#include <app_touchgfx.h>
#include <shared_data.h>
#include <cstring>

void TFTDisplay320x240::onTick()
{
	if (isOn()) {
		MX_TouchGFX_Process();
	}
}

void TFTDisplay320x240::toggle()
{
//	ILI9341_EnableSleepMode(isOn());
	togglePin();
}


void Display320x240::onTick()
{
	constexpr static const uint32_t kStep = 10;

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

	const auto& data = SharedData::getData<ProcessId::Idle>();

	DisplaySensitiveData newData{data.screenId, data.invState, data.usbState, data.chargerPlugged};
	if (data.getPowerModeState() == PowerModeState::Normal && m_sensData != newData)
	{
		m_sensData = newData;
		on();
	}
	else if (m_brightnessTimeout.isReached())
	{
		if (isOn()) {
			m_brightness = kOnRelaxed;
			m_brightnessTimeout.reset();
		} else {
			off();
		}
	}

	if (!isOff())
	{
		MX_TouchGFX_Process();
	}
}

void Display320x240::on()
{
	if (isOff()) {
		MX_TouchGFX_Process();
	}
	m_brightness = kOn;
	m_brightnessTimeout.reset();
}

void Display320x240::off()
{
	m_brightness = kOff;
}

void Display320x240::toggle()
{
	if (isOn()) {
		off();
	} else {
		on();
	}
}

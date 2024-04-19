/*
 * invertor_handler.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <inverter/inverter_handler.h>

InverterHandler::InverterHandler()
{}

void InverterHandler::onTick()
{
	m_btn.onTick();
	m_led.onTick();
}

void InverterHandler::handleEvents()
{
	m_btnHandler.handleEvents();

	if (m_isActive)
	{
		const auto currTick = HAL_GetTick();
		const auto onState = m_invOn.readPin();

		if (onState != m_invOk.readPin())
		{
			m_led.setIndicationType(LedIndicationType::FastBlinking);
			if (onState)
			{
				if (m_forceTurnOffTick < currTick) {
					m_invOn.togglePin();
				} else {
					m_stableStateCheckTick = currTick + 2200;
				}
			}

		}
		else if (m_stableStateCheckTick < currTick)
		{
			m_led.setIndicationType(onState ? LedIndicationType::On : LedIndicationType::Off);
			m_forceTurnOffTick = currTick + 8000;
		}
	}
}

void InverterHandler::setActive(bool active)
{
	if (m_isActive != active)
	{
		m_isActive = active;
		if (!active)
		{
			m_invOn.writePin(GPIO_PIN_RESET);
			m_led.setIndicationType(LedIndicationType::Off);
		}
	}
}

void InverterHandler::onBtnClicked()
{
	if (m_isActive)
	{
		m_invOn.togglePin();
		m_forceTurnOffTick = HAL_GetTick() + 8000;
	}
}



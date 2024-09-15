/*
 * fan_handler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */



#include <handlers/fan_handler.h>

FanHandler::FanHandler()
{
	m_changeValueDelay.setPaused(true);
	m_fanOn.writePin(m_currentFanValue > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void FanHandler::update()
{
	if (m_fanOn.readPin() || m_chargerDcOkPin.readPin() || m_invOk.readPin() || m_usbOn.readPin())
	{
		m_ntcHandler.onTick();
		updatePot(m_ntcHandler.getFanValue());
		m_fanOn.writePin(m_currentFanValue > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

void FanHandler::changeValue(bool increase)
{
	m_potINC.writePin(GPIO_PIN_SET);
	m_potCS1.writePin(GPIO_PIN_RESET);
	m_potUD.writePin(increase ? GPIO_PIN_RESET : GPIO_PIN_SET);
	m_changeValueDelay.reset();
	m_currentFanValue += increase ? 1 : -1;
}

void FanHandler::updatePot(uint8_t fanValue)
{
	if (fanValue < m_currentFanValue)
	{
		if (!m_fanExtraTime.isReached())
		{
			return;
		}
	}
	else
	{
		m_fanExtraTime.reset(20000);
	}

	if (m_changeValueDelay.isPaused())
	{
		if (fanValue != m_currentFanValue)
		{
			m_changeValueDelay.setPaused(false);
			changeValue(fanValue > m_currentFanValue);
		}
	}
	else if (m_changeValueDelay.isReached())
	{
		m_potINC.writePin(GPIO_PIN_RESET);
		m_potCS1.writePin(GPIO_PIN_SET);

		if (fanValue != m_currentFanValue)
		{
			changeValue(fanValue > m_currentFanValue);
		}
		else
		{
			//store
			m_potINC.writePin(GPIO_PIN_SET);
			m_potCS1.writePin(GPIO_PIN_RESET);
			HAL_Delay(10);
			m_potCS1.writePin(GPIO_PIN_SET);
			m_changeValueDelay.setPaused(true);
		}
	}
}



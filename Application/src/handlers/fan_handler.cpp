/*
 * fan_handler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#include <handlers/fan_handler.h>


void DigitalPotentiometer::changeValue(bool increase)
{
	m_potINC.writePin(GPIO_PIN_SET);
	m_potCS.writePin(GPIO_PIN_RESET);
	m_potUD.writePin(increase ? GPIO_PIN_SET : GPIO_PIN_RESET);
	m_changeValueDelay.reset();
	m_currentValue += increase ? 1 : -1;
}

void DigitalPotentiometer::setValue(uint8_t value)
{
	if (m_changeValueDelay.isPaused())
	{
		if (value != m_currentValue)
		{
			m_changeValueDelay.setPaused(false);
			changeValue(value > m_currentValue);
		}
	}
	else if (m_changeValueDelay.isReached())
	{
		m_potINC.writePin(GPIO_PIN_RESET);
		m_potCS.writePin(GPIO_PIN_SET);

		if (value != m_currentValue)
		{
			changeValue(value > m_currentValue);
		}
		else
		{
			//store
			m_potINC.writePin(GPIO_PIN_SET);
			m_potCS.writePin(GPIO_PIN_RESET);
			HAL_Delay(10);
			m_potCS.writePin(GPIO_PIN_SET);
			m_changeValueDelay.setPaused(true);
		}
	}
}


FanHandler::FanHandler()
{
	m_fanOn.writePin(m_potCS1.getValue() > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void FanHandler::update()
{
	if (isEnabled())
	{
		m_ntcHandler.onTick();
		updatePot(m_ntcHandler.getFanValue());
		m_fanOn.writePin(m_potCS1.getValue() > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

void FanHandler::updatePot(uint8_t fanValue)
{
	if (fanValue < m_potCS1.getValue())
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

	m_potCS1.setValue(fanValue);
}

bool FanHandler::isEnabled() const
{
	return m_fanOn.readPin() || m_chargerDcOkPin.readPin() == GPIO_PIN_RESET || m_invOk.readPin() || m_usbOn.readPin();
}

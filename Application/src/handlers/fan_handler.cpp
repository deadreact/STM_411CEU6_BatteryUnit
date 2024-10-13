/*
 * fan_handler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#include <handlers/fan_handler.h>


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

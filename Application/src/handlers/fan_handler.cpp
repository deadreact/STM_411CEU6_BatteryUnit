/*
 * fan_handler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#include <handlers/fan_handler.h>


FanHandler::FanHandler()
{
//	m_fanOn.writePin(m_potCS1.getValue() > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void FanHandler::update()
{
	m_fanOn.writePin(GPIO_PIN_RESET);
	TIM2->CCR2 = 999;

	// PWM 0   -> 2.049v
	// PWM 100 -> 1.926v
	// PWM 200 -> 1.820v
	// PWM 300 -> 1.696v
	// PWM 400 -> 1.515v
	// PWM 500 -> 3.054v
	// PWM 600 -> 4.942v
	// PWM 700 -> 6.810v
	// PWM 800 -> 8.670v
	// PWM 900 -> 10.52v
	// PWM 999 -> 12.06v


	if (isEnabled())
	{
		m_ntcHandler.onTick();
//		updatePot(m_ntcHandler.getFanValue());
//		m_fanOn.writePin(m_potCS1.getValue() > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
//	m_potCS1.onTick();
}

void FanHandler::updatePot(uint8_t fanValue)
{
//	if (fanValue < m_potCS1.getValue())
//	{
//		if (!m_fanExtraTime.isReached())
//		{
//			return;
//		}
//	}
//	else
//	{
//		m_fanExtraTime.reset(20000);
//	}

//	m_potCS1.setValue(fanValue);
}

bool FanHandler::isEnabled() const
{
//	return m_fanOn.readPin() || m_chargerDcOkPin.readPin() == GPIO_PIN_RESET || m_invOk.readPin() || m_usbOn.readPin();
	return false;
}

/*
 * digital_potentiometer.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/digital_potentiometer.h>


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


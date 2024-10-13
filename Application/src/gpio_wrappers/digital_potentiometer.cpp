/*
 * digital_potentiometer.cpp
 *
 *  Created on: Oct 12, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/digital_potentiometer.h>


void DigitalPotentiometer::onTick()
{
	if (m_csState == CSState::Deselection && m_deselectionTimeout.isReached())
	{
		m_csState = CSState::Unselected;
	}

	if (m_goalValue != m_currentValue)
	{
		changeValue(m_goalValue > m_currentValue);
	}
	else if (m_csState == CSState::Selected)
	{
		deselect();
	}
}

void DigitalPotentiometer::changeValue(bool increase)
{
	if (select() && m_incTimeout.isReached())
	{
		m_potUD.writePin(increase ? GPIO_PIN_SET : GPIO_PIN_RESET);
		m_potINC.togglePin();
		m_incTimeout.reset();

		if (m_potINC.readPin() == GPIO_PIN_RESET)
		{
			m_currentValue += increase ? 1 : -1;
		}
	}
}

bool DigitalPotentiometer::select()
{
	if (m_csState == CSState::Unselected)
	{
		m_potCS.writePin(GPIO_PIN_RESET);
		m_csState = CSState::Selected;
	}
	return m_csState == CSState::Selected;
}

void DigitalPotentiometer::deselect(bool store)
{
	if (m_csState == CSState::Selected)
	{
		m_potINC.writePin(store ? GPIO_PIN_SET : GPIO_PIN_RESET);
		m_potCS.writePin(GPIO_PIN_SET);
		m_csState = CSState::Deselection;
		m_deselectionTimeout.reset();
	}
}

void DigitalPotentiometer::setValue(uint8_t value)
{
	if (m_goalValue != value)
	{
		m_goalValue = value;
	}
}


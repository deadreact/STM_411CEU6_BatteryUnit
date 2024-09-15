/*
 * fan_handler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */



#include <handlers/fan_handler.h>

void FanHandler::X9C_decrease()
{
	m_potINC.writePin(GPIO_PIN_SET);
	m_potCS1.writePin(GPIO_PIN_RESET);
	m_potUD.writePin(GPIO_PIN_SET);
	HAL_Delay(10);
	m_potINC.writePin(GPIO_PIN_RESET);
	m_potCS1.writePin(GPIO_PIN_SET);
}
void FanHandler::X9C_increase()
{
	m_potINC.writePin(GPIO_PIN_SET);
	m_potCS1.writePin(GPIO_PIN_RESET);
	m_potUD.writePin(GPIO_PIN_RESET);
	HAL_Delay(10);
	m_potINC.writePin(GPIO_PIN_RESET);
	m_potCS1.writePin(GPIO_PIN_SET);
}
void FanHandler::X9C_store()
{
	m_potINC.writePin(GPIO_PIN_SET);
	m_potCS1.writePin(GPIO_PIN_RESET);
	HAL_Delay(10);
	m_potCS1.writePin(GPIO_PIN_SET);
}
//void FanHandler::X9C_dont_store()
//{
//	m_potCS1.writePin(GPIO_PIN_RESET);
//	m_potINC.writePin(GPIO_PIN_RESET);
//	HAL_Delay(10);
//	m_potCS1.writePin(GPIO_PIN_SET);
//}
//

void FanHandler::setDigipotVal(uint8_t val)
{

	if(m_currentFanValue > val)
	{
		for(int i = val; i <= m_currentFanValue; ++i)
		{
			X9C_decrease();
		}
	}
	else if(m_currentFanValue < val)
	{
		for(int i = m_currentFanValue; i <= val; ++i)
		{
			X9C_increase();
		}
	}
	else
    {
		return;
    }

	X9C_store();
	m_currentFanValue = val;
}

void FanHandler::update()
{
	m_ntcHandler.update();
	auto ev = m_ntcHandler.takeLastEvent();

	if (ev == NtcEvent::Updated)
	{
		m_fanValue = m_ntcHandler.getFanValue();
	}

	updatePot();

//	auto fanValue = m_ntcHandler.getFanValue();
//	setDigipotVal(fanValue);


	m_fanOn.writePin(m_currentFanValue > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);

}

void FanHandler::changeValue(bool increase)
{
	m_potINC.writePin(GPIO_PIN_SET);
	m_potCS1.writePin(GPIO_PIN_RESET);
	m_potUD.writePin(increase ? GPIO_PIN_RESET : GPIO_PIN_SET);
	m_delay.reset();
	m_currentFanValue += increase ? 1 : -1;
}

void FanHandler::updatePot()
{
	if (m_delay.isPaused())
	{
		if (m_fanValue != m_currentFanValue)
		{
			m_delay.setPaused(false);
			changeValue(m_fanValue > m_currentFanValue);
		}
	}
	else if (m_delay.isReached())
	{
		if (!m_potCS1.readPin())
		{
			m_potINC.writePin(GPIO_PIN_RESET);
			m_potCS1.writePin(GPIO_PIN_SET);

			if (m_fanValue != m_currentFanValue)
			{
				changeValue(m_fanValue > m_currentFanValue);
			}
			else
			{
				//store
				m_potINC.writePin(GPIO_PIN_SET);
				m_potCS1.writePin(GPIO_PIN_RESET);
				HAL_Delay(10);
				m_potCS1.writePin(GPIO_PIN_SET);
				m_delay.setPaused(true);
			}
		}
		else
		{
			m_delay.setPaused(true);
		}
	}
}



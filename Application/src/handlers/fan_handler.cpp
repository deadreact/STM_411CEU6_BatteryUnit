/*
 * fan_handler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#include <handlers/fan_handler.h>

static constexpr uint32_t minPWMValue = 400;
static constexpr uint32_t maxPWMValue = 999;
extern TIM_HandleTypeDef htim2;

FanHandler::FanHandler()
{
	m_fanOn.writePin(getFanPower() > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

uint8_t FanHandler::getFanPower() const
{
	return TIM2->CCR2 < minPWMValue ? 0 : ((TIM2->CCR2 - minPWMValue) * 100) / (maxPWMValue - minPWMValue);
}

void FanHandler::update()
{
//	// turn off fan
//	{
//		m_fanOn.writePin(GPIO_PIN_RESET);
//		TIM2->CCR2 = 400;
//	}
//
//	// turn on fan with min speed
//	{
//		m_fanOn.writePin(GPIO_PIN_SET);
//		TIM2->CCR2 = 560;
//	}
//
//	// turn on fan with max speed
//	{
//		m_fanOn.writePin(GPIO_PIN_SET);
//		TIM2->CCR2 = 999;
//	}
//
//	// turn off fan
//	{
//		m_fanOn.writePin(GPIO_PIN_RESET);
//		TIM2->CCR2 = 400;
//	}

	// PWM 0   -> 2.049v
	// PWM 100 -> 1.926v
	// PWM 200 -> 1.820v
	// PWM 300 -> 1.696v
	// PWM 400 -> 1.515v // turn off value
	// PWM 500 -> 3.054v
    // PWM 560 -> 4.091v // start value
	// PWM 600 -> 4.942v
	// PWM 700 -> 6.810v
	// PWM 800 -> 8.670v
	// PWM 900 -> 10.52v
	// PWM 999 -> 12.06v


	if (isEnabled())
	{
		m_ntcHandler.onTick();
		updatePot(m_ntcHandler.getFanValue());
		m_fanOn.writePin(getFanPower() > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

void FanHandler::updatePot(uint16_t fanValue)
{
	if (fanValue < TIM2->CCR2)
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

	if (TIM2->CCR2 != fanValue)
	{
		if (TIM2->CCR2 == 0)
		{
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
		}
		else if (fanValue == 0)
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
		}

		TIM2->CCR2 = fanValue;
	}
}

bool FanHandler::isEnabled() const
{
	return m_fanOn.readPin() || m_chargerDcOkPin.readPin() == GPIO_PIN_RESET || m_invOk.readPin() || m_usbOn.readPin();
}

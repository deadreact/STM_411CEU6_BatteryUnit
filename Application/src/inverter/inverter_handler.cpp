/*
 * invertor_handler.cpp
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#include <inverter/inverter_handler.h>

InverterHandler::InverterHandler()
{
//	m_btnHandler.setOnClickHandler([&]{ m_invOn.togglePin(); });
}

void InverterHandler::onTick()
{
	m_btn.onTick();
	m_led.onTick();

//	if (m_invFeedback.readPin()) {
//		m_lastOnFeedbackTick = HAL_GetTick();
//		m_stableFeedbackDuration = m_lastOnFeedbackTick - m_lastOffFeedbackTick;
//	} else {
//		m_lastOffFeedbackTick = HAL_GetTick();
//		m_stableFeedbackDuration = m_lastOffFeedbackTick - m_lastOnFeedbackTick;
//	}
}

void InverterHandler::handleEvents()
{
	m_btnHandler.handleEvents();
	
	const auto currTick = HAL_GetTick();
	const auto onState = m_invOn.readPin();

	if (onState != m_invFeedback.readPin())
	{
		m_led.setIndicationType(LedIndicationType::FastBlinking);
		m_stableStateCheckTick = currTick + 2200;

		if (onState && m_forceTurnOffTick < currTick) {
			m_invOn.togglePin();
		}
	}
	else if (m_stableStateCheckTick < currTick)
	{
		m_led.setIndicationType(onState ? LedIndicationType::On : LedIndicationType::Off);
		m_forceTurnOffTick = currTick + 8000;
	}


//	if (m_stableFeedbackDuration >= 1000) {
//		m_led.setIndicationType(m_lastOnFeedbackTick > m_lastOffFeedbackTick ? LedIndicationType::On : LedIndicationType::Off);
//	} else {
//		m_led.setIndicationType(LedIndicationType::FastBlinking);
//	}
}

void InverterHandler::onBtnClicked()
{
	m_invOn.togglePin();
	m_forceTurnOffTick = HAL_GetTick() + 8000;
}



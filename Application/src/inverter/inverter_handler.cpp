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

	if (m_invFeedback.readPin()) {
		m_lastOnFeedbackTick = HAL_GetTick();
		m_stableFeedbackDuration = m_lastOnFeedbackTick - m_lastOffFeedbackTick;
	} else {
		m_lastOffFeedbackTick = HAL_GetTick();
		m_stableFeedbackDuration = m_lastOffFeedbackTick - m_lastOnFeedbackTick;
	}
}

void InverterHandler::handleEvents()
{
	m_btnHandler.handleEvents();
	
	if (m_stableFeedbackDuration >= 1000) {
		m_led.setIndicationType(m_lastOnFeedbackTick > m_lastOffFeedbackTick ? LedIndicationType::On : LedIndicationType::Off);
	} else {
		m_led.setIndicationType(LedIndicationType::FastBlinking);
	}
}

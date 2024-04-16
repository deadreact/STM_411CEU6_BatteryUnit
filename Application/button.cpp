/*
 * button.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include "button.h"

Button::Button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
    : SinglePinElement(GPIOx, GPIO_Pin)
    , m_lastChangeTick(HAL_GetTick())
    , m_lastStateChangeTick(m_lastChangeTick)
    , m_lastPinState(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))
    , m_state(m_lastPinState)
{}

void Button::onTick()
{
    const auto state = HAL_GPIO_ReadPin(m_GPIOx, m_pin);

    if (state != m_lastPinState)
    {
        m_lastPinState = state;
        m_lastChangeTick = HAL_GetTick();
    }
    else if (state != m_state)
    {
        if (HAL_GetTick() - m_lastChangeTick > kTresholdMs)
        {
            m_state = state;
            m_lastStateChangeTick = HAL_GetTick() - 1;
        }
    }

}

bool Button::isPressed() const { return m_state == 0; }
uint32_t Button::getPressedDuration() const { return isPressed() ? HAL_GetTick() - m_lastStateChangeTick : 0; }

ButtonEventProvider::ButtonEventProvider(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
    : Button(GPIOx, GPIO_Pin)
    , m_pressedDuration(getPressedDuration())
{}

void ButtonEventProvider::onTick()
{
    static const uint32_t firstHoldTreshold = 400;
    static const uint32_t nextHoldTreshold = 80;

    const uint32_t holdTreshold = m_firstHoldTriggered ? nextHoldTreshold : firstHoldTreshold;

//    m_lastEvent = ButtonEvent::NoEvent;
    Button::onTick();

    auto currPressedDuration = getPressedDuration();
    if (m_pressedDuration != currPressedDuration)
    {
        if (currPressedDuration == 0)
        {
            m_lastEvent = ButtonEvent::Release;
            m_pressedDuration = currPressedDuration;
            m_firstHoldTriggered = false;
        }
        else if (m_pressedDuration == 0)
        {
            m_lastEvent = ButtonEvent::Press;
            m_pressedDuration = currPressedDuration;
            m_firstHoldTriggered = false;
        }
        else if (currPressedDuration - m_pressedDuration > holdTreshold)
        {
            m_lastEvent = ButtonEvent::Hold;
            m_pressedDuration = currPressedDuration;
            m_firstHoldTriggered = true;
        }
    }
}

ButtonEvent ButtonEventProvider::takeLastEvent()
{
	auto tmp = m_lastEvent;
	m_lastEvent = ButtonEvent::NoEvent;
	return tmp;
}

ButtonEventHandler::ButtonEventHandler(ButtonEventProvider* eventProvider, function_t onClick, function_t onHold)
	: m_eventProvider(eventProvider)
	, m_onClick(onClick)
	, m_onHold(onHold)
{}

void ButtonEventHandler::handleEvent(ButtonEvent event)
{
	switch (event)
	{
		case ButtonEvent::Release:
		{
			if (m_lastEvent == ButtonEvent::Press) {
				if (m_onClick) m_onClick();
			}
			m_lastEvent = event;

		} break;
		case ButtonEvent::Press:
			m_lastEvent = event;
			break;
		case ButtonEvent::Hold:
		{
			if (m_onHold) m_onHold();
			m_lastEvent = event;
		} break;
		default:
			break;
	}
}

void ButtonEventHandler::handleEvents()
{
	if (m_eventProvider)
	{
		handleEvent(m_eventProvider->takeLastEvent());
	}
}

void ButtonEventHandler::setEventProvider(ButtonEventProvider* eventProvider)
{
	if (m_eventProvider != eventProvider) {
		m_eventProvider = eventProvider;
		m_lastEvent = ButtonEvent::NoEvent;
	}
}


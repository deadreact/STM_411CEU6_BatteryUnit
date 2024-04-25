/*
 * led.cpp
 *
 *  Created on: Apr 16, 2024
 *      Author: deadreact
 */

#include <gpio_wrappers/led.h>

Led::Led(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
    : SinglePinElement(GPIOx, GPIO_Pin)
{}

void Led::onTick()
{
    if (m_timeout.isReached())
    {
    	m_timeout.reset();
        toggle();
    }
}

void Led::reset()
{
	m_timeout.invalidate();
}

void Led::setInterval(uint32_t interval) { m_timeout = interval; }
uint32_t Led::getInterval() const { return m_timeout.getTimeout(); }

LedIndicator::LedIndicator(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, LedIndicationType initialState)
    : Led(GPIOx, GPIO_Pin)
    , m_indication(initialState)
{
    setupIndication();
}

void LedIndicator::onTick()
{
    Led::onTick();

    if (m_indication == LedIndicationType::ShuffleBlinking && m_timeout.getStartTick() == HAL_GetTick())
    {
        if (isOn())
        {
            m_timeout = m_timeout.getTimeout() * 2;
        }
        else
        {
        	m_timeout = m_timeout.getTimeout() / 2;
        }
    }
}

void LedIndicator::setIndicationType(LedIndicationType indication)
{
    if (m_indication != indication)
    {
        m_indication = indication;

        setupIndication();
    }
}

void LedIndicator::setupIndication()
{
    switch (m_indication)
    {
        case LedIndicationType::Off:
        {
            m_timeout.invalidate();
            off();
        } break;
        case LedIndicationType::On:
        {
            m_timeout.invalidate();
            on();
        } break;
        case LedIndicationType::Blinking:
        case LedIndicationType::FastBlinking:
        case LedIndicationType::ShuffleBlinking:
        {
            setInterval(static_cast<uint32_t>(m_indication));
        } break;
        default:
            break;
    }
}


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
    const auto dt = HAL_GetTick() - m_lastToggleTick;
    if (dt > m_interval)
    {
        m_lastToggleTick = HAL_GetTick();
        toggle();
    }
}

void Led::reset()
{
    m_interval = 0xffffffff;
    m_lastToggleTick = 0;
}

void Led::setInterval(uint32_t interval) { m_interval = interval; }
uint32_t Led::getInterval() const { return m_interval; }

LedIndicator::LedIndicator(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, LedIndicationType initialState)
    : Led(GPIOx, GPIO_Pin)
    , m_indication(initialState)
{
    setupIndication();
}

void LedIndicator::onTick()
{
    Led::onTick();

    if (m_indication == LedIndicationType::ShuffleBlinking && m_lastToggleTick == HAL_GetTick())
    {
        if (isOn())
        {
            m_lastToggleTick -= m_interval / 2;
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
            setInterval(0xffffffff);
            off();
        } break;
        case LedIndicationType::On:
        {
            setInterval(0xffffffff);
            on();
        } break;
        case LedIndicationType::Blinking:
        {
            setInterval(1000);
        } break;
        case LedIndicationType::FastBlinking:
        {
            setInterval(200);
        } break;
        case LedIndicationType::ShuffleBlinking:
        {
            setInterval(266);
        } break;
        default:
            break;
    }
}


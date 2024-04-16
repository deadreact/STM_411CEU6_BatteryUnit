/*
 * led.cpp
 *
 *  Created on: Apr 16, 2024
 *      Author: deadreact
 */

#include "led.h"

Led::Led(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
    : SinglePinElement(GPIOx, GPIO_Pin)
{
    on();
}

void Led::onTick()
{
    const auto dt = HAL_GetTick() - m_lastToggleTick;
    if (dt > m_interval)
    {
        m_lastToggleTick += m_interval;
        toggle();
    }
}

void Led::on() const { HAL_GPIO_WritePin(m_GPIOx, m_pin, GPIO_PIN_SET); }
void Led::off() const { HAL_GPIO_WritePin(m_GPIOx, m_pin, GPIO_PIN_RESET); }
void Led::toggle() const { HAL_GPIO_TogglePin(m_GPIOx, m_pin); }

void Led::reset()
{
    m_interval = 0xffffffff;
    m_lastToggleTick = HAL_GetTick();
    on();
}

void Led::setInterval(uint16_t interval) { m_interval = interval; }
uint16_t Led::getInterval() const { return m_interval; }

/*
 * Interface.h
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef PIN_WPAPPER_H_
#define PIN_WPAPPER_H_

#include "stm32f4xx_hal.h"

/**
 * @brief      Wrapper on GPIO single pin
 */
class PinWrapper
{
public:
    PinWrapper(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
        : m_GPIOx(GPIOx)
        , m_pin(GPIO_Pin)
    {}

    inline GPIO_PinState readPin() const { return HAL_GPIO_ReadPin(m_GPIOx, m_pin); }
    inline void writePin(GPIO_PinState state) { return HAL_GPIO_WritePin(m_GPIOx, m_pin, state); }
    inline void togglePin() { return HAL_GPIO_TogglePin(m_GPIOx, m_pin); }
protected:
    GPIO_TypeDef* const m_GPIOx;
    const uint16_t m_pin;
};

#endif /* PIN_WPAPPER_H_ */

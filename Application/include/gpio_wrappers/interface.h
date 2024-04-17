/*
 * Interface.h
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "stm32f4xx_hal.h"

class SinglePinElement
{
public:
    SinglePinElement(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
        : m_GPIOx(GPIOx)
        , m_pin(GPIO_Pin)
    {}
protected:
    GPIO_TypeDef* const m_GPIOx;
    const uint16_t m_pin;
};


class ITickHandler
{
public:
    virtual ~ITickHandler() {}
    virtual void onTick() = 0;
};


#endif /* INTERFACE_H_ */

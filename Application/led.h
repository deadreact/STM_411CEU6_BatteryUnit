/*
 * led.h
 *
 *  Created on: Apr 16, 2024
 *      Author: deadreact
 */

#ifndef LED_H_
#define LED_H_

#include "interface.h"

class Led : public SinglePinElement, public ITickHandler
{
public:
    Led(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

    virtual void onTick() override;

    void on() const;
    void off() const;
    void toggle() const;
    void reset();

    void setInterval(uint16_t interval);
    uint16_t getInterval() const;
private:
    uint32_t m_lastToggleTick{0};
    uint32_t m_interval{0xffffffff};
};

#endif /* LED_H_ */

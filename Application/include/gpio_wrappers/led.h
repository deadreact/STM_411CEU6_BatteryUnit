/*
 * led.h
 *
 *  Created on: Apr 16, 2024
 *      Author: deadreact
 */

#ifndef LED_H_
#define LED_H_

#include <gpio_wrappers/interface.h>
#include <utils/timeout.h>

class Led : public SinglePinElement, public ITickHandler
{
public:
    Led(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

    virtual void onTick() override;

    inline void on() { writePin(GPIO_PIN_SET); }
    inline void off() { writePin(GPIO_PIN_RESET); }
    inline void toggle() { togglePin(); }
    void reset();

    inline bool isOn() const { return readPin(); }

    void setInterval(uint32_t interval);
    uint32_t getInterval() const;
protected:
    Timeout m_timeout;
};

enum class LedIndicationType
{
    Off = 0,
    On,
    Blinking,
    FastBlinking,
    ShuffleBlinking,

    Count
};

class LedIndicator : public Led
{
public:
    LedIndicator(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, LedIndicationType initialState = LedIndicationType::Off);

    virtual void onTick() override;

    void setIndicationType(LedIndicationType indication);
    LedIndicationType getIndicationType() const { return m_indication; }
private:
    void setupIndication();
private:
    LedIndicationType m_indication{LedIndicationType::Off};
};

#endif /* LED_H_ */

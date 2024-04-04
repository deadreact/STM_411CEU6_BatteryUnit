/*
 * button.h
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "interface.h"

class Button : public SinglePinElement, public ITickHandler
{
	constexpr static const uint32_t kTresholdMs = 50;
public:
	Button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

    virtual void onTick() override;
    bool isPressed() const;
    uint32_t getPressedDuration() const;
private:
    uint32_t m_lastChangeTick {0};
    uint32_t m_lastStateChangeTick {0};
    GPIO_PinState m_lastPinState;
    GPIO_PinState m_state;
};

enum class ButtonEvent : uint8_t
{
	NoEvent,
	Press,
	Release,
	Hold,
	Unexpected
};

class ButtonEventProvider : public ITickHandler
{
public:
	ButtonEventProvider(Button&& btn);
	virtual void onTick() override;
	virtual void onEvent(ButtonEvent e) { m_lastEvent = e; }
	ButtonEvent getLastEvent() const { return m_lastEvent; }
private:
	Button m_btn;
	uint32_t m_pressedDuration;
	ButtonEvent m_lastEvent;
};



#endif /* BUTTON_H_ */

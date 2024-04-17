/*
 * button.h
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include <gpio_wrappers/interface.h>
#include <functional>

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
    Hold
};

class ButtonEventProvider : public Button
{
public:
    ButtonEventProvider(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t firstHoldTreshold = 800, uint32_t nextHoldTreshold = 0xffffffff/*disable*/);

    virtual void onTick() override;
    ButtonEvent takeLastEvent();

    void setupHoldTriggerTimeouts(uint32_t firstHoldTreshold, uint32_t nextHoldTreshold = 0xffffffff/*disable*/);
private:
    uint32_t m_pressedDuration;
    ButtonEvent m_lastEvent {ButtonEvent::NoEvent};
    bool m_firstHoldTriggered{false};

    // config
    uint32_t m_firstHoldTreshold{800};
    uint32_t m_nextHoldTreshold{0xffffffff};
};

class ButtonEventHandler
{
public:
    using Fn = std::function<void()>;

    ButtonEventHandler(ButtonEventProvider* eventProvider = nullptr, Fn onClick = nullptr, Fn onHold = nullptr);

    virtual void handleEvent(ButtonEvent event);
    void handleEvents();

    void setEventProvider(ButtonEventProvider* eventProvider);
    void setOnClickHandler(Fn handler) { m_onClick = handler; }
    void setOnHoldHandler(Fn handler) { m_onHold = handler; }
protected:
    ButtonEventProvider* m_eventProvider{nullptr};
    Fn m_onClick{nullptr};
    Fn m_onHold{nullptr};
    ButtonEvent m_lastEvent{ButtonEvent::NoEvent};
};


class PwrButtonEventHandler: public ButtonEventHandler
{
public:
	PwrButtonEventHandler(ButtonEventProvider* ep = nullptr, Fn onClick = nullptr, Fn onHold = nullptr, Fn onPress = nullptr, Fn onRealease = nullptr);

	virtual void handleEvent(ButtonEvent event) override;

	void setOnPressHandler(Fn handler) { m_onPress = handler; }
	void setOnReleaseHandler(Fn handler) { m_onRelease = handler; }
protected:
	Fn m_onPress{nullptr};
	Fn m_onRelease{nullptr};
};

#endif /* BUTTON_H_ */

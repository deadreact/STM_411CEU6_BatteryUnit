/*
 * button.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include <gpio_wrappers/button.h>
#include <utility>

namespace
{
//    template <typename C, typename R>
//    R get_method_return_type(R(C::*)()const);

    template <typename F, typename... Args>
    void execute(const F& fn, Args&&... args) {
        fn(std::forward(args)...);
    }

    template <typename... Args>
    void execute(void(*fn)(Args...), Args... args) {
        (*fn)(args...);
    }
}

Button::Button(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
    : PinWrapper(GPIOx, GPIO_Pin)
    , m_lastStateChangeTick(HAL_GetTick())
    , m_lastPinState(readPin())
    , m_state(m_lastPinState)
{}

void Button::onTick()
{
    const auto state = readPin();

    if (state != m_lastPinState)
    {
        m_lastPinState = state;
        m_lastChangeTimeout.reset();
    }
    else if (state != m_state)
    {
        if (m_lastChangeTimeout.isReached())
        {
            m_state = state;
            m_lastStateChangeTick = HAL_GetTick() - 1;
        }
    }

}

bool Button::isPressed() const { return m_state == 0; }
uint32_t Button::getPressedDuration() const { return isPressed() ? HAL_GetTick() - m_lastStateChangeTick : 0; }

ButtonEventProvider::ButtonEventProvider(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t firstHoldTreshold, uint32_t nextHoldTreshold)
    : Button(GPIOx, GPIO_Pin)
    , m_pressedDuration(getPressedDuration())
    , m_firstHoldTreshold(firstHoldTreshold)
    , m_nextHoldTreshold(nextHoldTreshold)
{}

void ButtonEventProvider::onTick()
{
    const uint32_t holdTreshold = m_firstHoldTriggered ? m_nextHoldTreshold : m_firstHoldTreshold;

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

void ButtonEventProvider::setupHoldTriggerTimeouts(uint32_t firstHoldTreshold, uint32_t nextHoldTreshold)
{
    m_firstHoldTreshold = firstHoldTreshold;
    m_nextHoldTreshold = nextHoldTreshold;
}

ButtonEventHandler::ButtonEventHandler(ButtonEventProvider* eventProvider, Fn onClick, Fn onHold)
    : m_eventProvider(eventProvider)
    , m_onClick(onClick)
    , m_onHold(onHold)
{}

void ButtonEventHandler::handleEvent(ButtonEvent event)
{
    if (event == ButtonEvent::NoEvent) {
        return;
    }

    switch (event)
    {
        case ButtonEvent::Release:
        {
            if (m_lastEvent == ButtonEvent::Press) {
                if (m_onClick) execute(m_onClick);
            }
            m_lastEvent = event;

        } break;
        case ButtonEvent::Press:
            m_lastEvent = event;
            break;
        case ButtonEvent::Hold:
        {
            if (m_onHold) execute(m_onHold);
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

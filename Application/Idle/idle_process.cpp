/*
 * idle_process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "idle_process.h"
#include "power_modes.h"
#include <shared_data.h>
#include "app_touchgfx.h"
#include "stm32f4xx_hal.h"
#include "../button.h"
#include "main.h"
#include "../BMS/bms_handler.h"
#include <stdio.h>

// ----------------- tmp here --------------------------
class ScreenBrightnessController
{
public:
    ScreenBrightnessController(int& brightness)
        : brightness(brightness)
    {
        ApplyBrightness();
    }

    void toggleScreen() {
        screenOn = !screenOn;
        ApplyBrightness();

        if (!screenOn)
        {
//            __disable_irq();
//            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
//            __enable_irq();
        }
    }

    void incBrightness() {
        brightness = (brightness + 40) % 1000;
        ApplyBrightness();
    }
private:
    void ApplyBrightness() {
        // brightnessHandle = screenOn ? brightness : 0;
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, screenOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }
private:
    int& brightness;
    // volatile uint32_t& brightnessHandle {TIM2->CCR4};
    bool screenOn {true};
    ButtonEvent lastEvent {ButtonEvent::NoEvent};
};

class KeyButtonToScreenHandler
{
public:
    KeyButtonToScreenHandler(int& brightness): controller(brightness) {}

    void handleEvent(ButtonEvent event)
    {
        switch (event)
        {
            case ButtonEvent::Release:
            {
                if (lastEvent == ButtonEvent::Press) {
                    controller.toggleScreen();
                }
                lastEvent = event;

            } break;
            case ButtonEvent::Press:
                lastEvent = event;
                break;
            case ButtonEvent::Hold:
            {
                controller.incBrightness();
                lastEvent = event;
            } break;
            default:
                break;
        }
    }
private:
    ScreenBrightnessController controller;
    ButtonEvent lastEvent{ButtonEvent::NoEvent};
};

// ---------------------------------------------------------------
class BtnSleepHandler
{
public:
    BtnSleepHandler(int& sleepingMode): sleepingMode(sleepingMode) {}
    void handleEvent(ButtonEvent event)
    {
        switch (event)
        {
            case ButtonEvent::Release:
            {
                if (lastEvent == ButtonEvent::Press) {
                    sleepingMode = 1;
                }
                lastEvent = event;

            } break;
            case ButtonEvent::Press:
            case ButtonEvent::Hold:
                lastEvent = event;
                break;
            default:
                break;
        }
    }
private:
    int& sleepingMode;
    ButtonEvent lastEvent{ButtonEvent::NoEvent};
};
// ---------------------------------------------------------------

struct IdleProcess::Impl
{
    void OnTick();
    void HandleEvents();

    void UpdateAnalog();

    ProcessData<ProcessId::Idle> sharedData;

    BMSUpdater m_bmsUpdater;

//    ButtonEventProvider btnScreenOn{GPIOA, GPIO_PIN_12};
    ButtonEventProvider btnEnterSleep{GPIOB, GPIO_PIN_6};
    ButtonEventProvider btnBmsToggle{GPIOA, GPIO_PIN_12};
    KeyButtonToScreenHandler btnScreenOnHandler{sharedData.screenBrightness};
    BtnSleepHandler btnSleepHandler{sharedData.sleepingMode};
};

extern ADC_HandleTypeDef hadc1;

void IdleProcess::Impl::UpdateAnalog()
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	sConfig.Channel = ADC_CHANNEL_1;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 500);
	sharedData.analog1 = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	sConfig.Channel = ADC_CHANNEL_4;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 500);
	sharedData.analog2 = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
}



void IdleProcess::Impl::OnTick()
{
    if (sharedData.sleepingMode) {
        return;
    }
//    btnScreenOn.onTick();
    btnEnterSleep.onTick();
    btnBmsToggle.onTick();

    if (sharedData.bmsActive) {
    	m_bmsUpdater.Update();
    } else {
    	UpdateAnalog();
    }
}

void IdleProcess::Impl::HandleEvents()
{
//    btnScreenOnHandler.handleEvent(btnScreenOn.getLastEvent());
    btnSleepHandler.handleEvent(btnEnterSleep.getLastEvent());

    if (btnBmsToggle.getLastEvent() == ButtonEvent::Release) {
    	sharedData.bmsActive = !sharedData.bmsActive;
    	if (sharedData.bmsActive)
    	{
    		m_bmsUpdater.Request();
    	}
    }

    BMSUpdaterEvent bmsEvent = m_bmsUpdater.GetLastEvent();
    if (bmsEvent != BMSUpdaterEvent::NoEvent)
    {
    	const auto& data = m_bmsUpdater.GetData();
		sharedData.batCurrent = data.current * 0.01f;
		sharedData.batCapacity = data.capacity;
		sharedData.batVoltage = data.voltage;
		sharedData.errMsg =  m_bmsUpdater.debugMsg;
    }
}


// -----------------------------------------------------------------

IdleProcess::~IdleProcess()
{
    delete m_pimpl;
}

void IdleProcess::Init()
{
    m_pimpl = new IdleProcess::Impl;
    SharedData::get().setProcessId<ProcessId::Idle>(&m_pimpl->sharedData);

    m_tickRate = 5;
}

void IdleProcess::Update()
{
	printf("!!!!!!!!!!!\n");

    bool gotoSleep = m_pimpl->sharedData.sleepingMode > 1;

    m_pimpl->OnTick();
    MX_TouchGFX_Process();
    m_pimpl->HandleEvents();

    if (gotoSleep)
    {
        EnterStopMode();
        m_pimpl->sharedData.sleepingMode = 0;
    }
}



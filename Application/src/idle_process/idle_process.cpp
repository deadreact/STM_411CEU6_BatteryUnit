/*
 * idle_process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "../idle_process/idle_process.h"

#include "power_modes.h"
#include "app_touchgfx.h"
#include "stm32f4xx_hal.h"
#include <gpio_wrappers/button.h>
#include <gpio_wrappers/led.h>
#include <gpio_wrappers/io_tube.h>
#include "main.h"
#include <cstring>
#include <ili9341.h>
#include <shared_data.h>

#include <bms/bms_handler.h>

class Screen : protected SinglePinElement, public ITickHandler
{
public:
	using SinglePinElement::SinglePinElement;

	virtual void onTick() override
	{
		if (readPin()) {
			MX_TouchGFX_Process();
		}
	}

	void toggle()
	{
		ILI9341_EnableSleepMode(readPin());
		HAL_GPIO_TogglePin(m_GPIOx, m_pin);
	}

	void on() { if (!readPin()) toggle(); }
	void off() { if (readPin()) toggle(); }
};

// ---------------------------------------------------------------

struct IdleProcess::Impl
{
    void OnTick();
    void HandleEvents();

    void UpdateAnalog();

    void OnPwrClick();
    void OnPwrHold();
    void OnPwrPress();
    void OnPwrRelease();

    // Data
    ProcessData<ProcessId::Idle> sharedData;
//    bool screenOn{true};

    // Handlers
    BMSUpdater m_bmsUpdater;

    Screen screen{LED_GPIO_Port, LED_Pin};
    LedIndicator screenLed{bttn_screen_led_GPIO_Port, bttn_screen_led_Pin, LedIndicationType::Blinking};
    ButtonEventProvider btnEnterSleep{GPIOB, GPIO_PIN_6};
    ButtonEventProvider btnBmsToggle{GPIOB, GPIO_PIN_3, 800, 800};
    ButtonEventProvider btnPwr{bttn_screen_on_GPIO_Port, bttn_screen_on_Pin};
    ButtonEventHandler btnSleepHandler{&btnEnterSleep, [&]{ sharedData.powerModeState = PowerModeState::StopRequested; }};
    ButtonEventHandler btnScrSwitchHandler{&btnBmsToggle
   	, [&]{ sharedData.screenId = (sharedData.screenId + 1) % 3; }
//    , [&]{ led.setIndicationType(LedIndicationType(((int)led.getIndicationType() + 1) % int(LedIndicationType::Count))); }
    };

    PwrButtonEventHandler btnPwrHandler{&btnPwr, [&]{ OnPwrClick();}, [&]{ OnPwrHold();}, [&]{ OnPwrPress();}, [&]{ OnPwrRelease();}};

    IOTube boardLedTube{{bms_ok_GPIO_Port, bms_ok_Pin}, {GPIOC, GPIO_PIN_13}};

    IOTube invertorTube{{inv_ok_GPIO_Port, inv_ok_Pin}, {bttn_inv_led_GPIO_Port, bttn_inv_led_Pin}};
    IOTube usbTube{{usb_on_GPIO_Port, usb_on_Pin}, {bttn_usb_led_GPIO_Port, bttn_usb_led_Pin}};
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

	screenLed.onTick();
//    led.onTick();
    boardLedTube.onTick();
    btnEnterSleep.onTick();
    btnBmsToggle.onTick();
    btnPwr.onTick();

    invertorTube.onTick();
    usbTube.onTick();

    if (sharedData.powerModeState == PowerModeState::StopRequested) {
		m_bmsUpdater.UpdateAndStop();
//		return;
	} else
    if (sharedData.screenId != 2) {
        m_bmsUpdater.Update();
    } else {
        UpdateAnalog();
    }

    screen.onTick();
}

void IdleProcess::Impl::HandleEvents()
{
    btnSleepHandler.handleEvents();
    btnScrSwitchHandler.handleEvents();
    btnPwrHandler.handleEvents();

    BMSUpdaterEvent bmsEvent = m_bmsUpdater.GetLastEvent();
    if (bmsEvent != BMSUpdaterEvent::NoEvent)
    {
        sharedData.bms = m_bmsUpdater.GetData();
        sharedData.bmsErrMsg = m_bmsUpdater.debugMsg;
        sharedData.bmsErrFlags = m_bmsUpdater.errFlags;
    }

    if (sharedData.powerModeState == PowerModeState::StopRequested)
    {
    	screenLed.setIndicationType(LedIndicationType::FastBlinking);
        if (m_bmsUpdater.GetStatus() == BMSStatus::NoStatus)
        {
        	if (NVIC_GetEnableIRQ(bttn_screen_on_EXTI_IRQn))
        	{
        		screen.off();
        		screenLed.setIndicationType(LedIndicationType::Off);
        		sharedData.powerModeState = PowerModeState::StopReady;
        	}
        }
    }
    else
    {
    	if (m_bmsUpdater.isPowerOn())
    	{
    		if (m_bmsUpdater.GetStatus() != BMSStatus::Error && m_bmsUpdater.GetData().isValid())
    		{
    			screenLed.setIndicationType(LedIndicationType::On);
    		}
    		else
    		{
    			screenLed.setIndicationType(LedIndicationType::ShuffleBlinking);
    		}
    	}
    	else
    	{
    		screenLed.setIndicationType(LedIndicationType::Off);
    	}
    }
}


void IdleProcess::Impl::OnPwrPress()
{
	HAL_NVIC_DisableIRQ(bttn_screen_on_EXTI_IRQn);
}

void IdleProcess::Impl::OnPwrRelease()
{
	HAL_NVIC_EnableIRQ(bttn_screen_on_EXTI_IRQn);
}

void IdleProcess::Impl::OnPwrClick()
{
	screen.toggle();
}

void IdleProcess::Impl::OnPwrHold()
{
	sharedData.powerModeState = PowerModeState::StopRequested;
	screen.off();
}


// -----------------------------------------------------------------
//static void RequestStopMode() { SharedData:: }
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
    m_pimpl->screen.on();
}

void IdleProcess::Update()
{
//    bool gotoSleep = m_pimpl->sharedData.sleepingMode > 1;

    m_pimpl->OnTick();
//    if (m_pimpl->screen.readPin()) {
//    	MX_TouchGFX_Process();
//    }
    m_pimpl->HandleEvents();

    if (m_pimpl->sharedData.powerModeState == PowerModeState::StopReady)
    {
        EnterStopMode();
//        m_pimpl->sharedData.sleepingMode = 0;
        m_pimpl->sharedData.powerModeState = PowerModeState::WakedUp;
        m_pimpl->screen.on();
    }
}



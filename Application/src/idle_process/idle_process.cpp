/*
 * idle_process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "idle_process.h"

#include "power_modes.h"
#include <gpio_wrappers/button.h>
#include <gpio_wrappers/led.h>
#include <gpio_wrappers/io_tube.h>
#include <gpio_wrappers/tft_display_320x240.h>
#include "main.h"
#include <cstring>
#include <shared_data.h>

#include <bms/bms_handler.h>
#include <inverter/inverter_handler.h>

// ---------------------------------------------------------------

struct IdleProcess::Impl
{
    void onTick();
    void handleEvents();

    void handlePowerState();

    void updateAnalog();

    void onPwrClick();
    void onPwrHold();
    void onPwrPress();
    void onPwrRelease();

    // Data
    ProcessData<ProcessId::Idle> sharedData;
    // Handlers
    BMSUpdater m_bmsUpdater;
    InverterHandler m_invHandler;

    TFTDisplay320x240 screen{LED_GPIO_Port, LED_Pin};
    LedIndicator screenLed{bttn_screen_led_GPIO_Port, bttn_screen_led_Pin, LedIndicationType::Blinking};
//    ButtonEventProvider btnEnterSleep{GPIOB, GPIO_PIN_6};
    ButtonEventProvider btnBmsToggle{GPIOB, GPIO_PIN_3, 800, 800};
    ButtonEventProvider btnPwr{bttn_screen_on_GPIO_Port, bttn_screen_on_Pin};
//    ButtonEventHandler btnSleepHandler{&btnEnterSleep, [&]{ sharedData.powerModeState = PowerModeState::StopRequested; }};
    ButtonEventHandler btnScrSwitchHandler{&btnBmsToggle
   	, [&]{ sharedData.screenId = (sharedData.screenId + 1) % 3; }
//    , [&]{ led.setIndicationType(LedIndicationType(((int)led.getIndicationType() + 1) % int(LedIndicationType::Count))); }
    };

    ButtonEventHandler btnPwrHandler{&btnPwr, [&]{ onPwrClick();}, [&]{ onPwrHold();}};

    IOTube boardLedTube{{bms_ok_GPIO_Port, bms_ok_Pin}, {GPIOC, GPIO_PIN_13}};

//    IOTube invertorTube{{inv_ok_GPIO_Port, inv_ok_Pin}, {bttn_inv_led_GPIO_Port, bttn_inv_led_Pin}};
    IOTube usbTube{{usb_on_GPIO_Port, usb_on_Pin}, {bttn_usb_led_GPIO_Port, bttn_usb_led_Pin}};
};

extern ADC_HandleTypeDef hadc1;

void IdleProcess::Impl::updateAnalog()
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



void IdleProcess::Impl::onTick()
{
	m_invHandler.onTick();
	screenLed.onTick();
//    led.onTick();
    boardLedTube.onTick();
//    btnEnterSleep.onTick();
    btnBmsToggle.onTick();
    btnPwr.onTick();

//    invertorTube.onTick();
    usbTube.onTick();

    m_bmsUpdater.update();

    if (sharedData.screenId == 2) {
        updateAnalog();
    }

    screen.onTick();
}

void IdleProcess::Impl::handleEvents()
{
//    btnSleepHandler.handleEvents();
    btnScrSwitchHandler.handleEvents();
    btnPwrHandler.handleEvents();
    m_invHandler.handleEvents();

    BMSUpdaterEvent bmsEvent = m_bmsUpdater.takeLastEvent();
    if (bmsEvent != BMSUpdaterEvent::NoEvent)
    {
        sharedData.bms = m_bmsUpdater.getData();
        sharedData.bmsErrMsg = m_bmsUpdater.debugMsg;
        sharedData.bmsErrFlags = m_bmsUpdater.errFlags;

        if (bmsEvent == BMSUpdaterEvent::DataUpdated) {
        	sharedData.smoothedCurrent.set(sharedData.bms.current);
        }
    }

    handlePowerState();
}

void IdleProcess::Impl::handlePowerState()
{
	if (sharedData.powerModeState == PowerModeState::StopRequested)
	{
		m_bmsUpdater.setActive(false);
		screenLed.setIndicationType(LedIndicationType::FastBlinking);
		if (m_bmsUpdater.getStatus() == BMSStatus::NoStatus)
		{
//        	if (NVIC_GetEnableIRQ(bttn_screen_on_EXTI_IRQn))
			if (!btnPwr.isPressed())
			{
				screen.off();
				screenLed.setIndicationType(LedIndicationType::Off);
				sharedData.powerModeState = PowerModeState::StopReady;
			}
		}
	}
	else
	{
		m_bmsUpdater.setActive(true);
		if (sharedData.powerModeState == PowerModeState::WakedUp)
		{
			if (!btnPwr.isPressed())
			{
				sharedData.powerModeState = PowerModeState::Normal;
			}
		}

		if (m_bmsUpdater.isPowerOn())
		{
			if (m_bmsUpdater.getStatus() != BMSStatus::Error && m_bmsUpdater.getData().isValid())
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


void IdleProcess::Impl::onPwrPress()
{
//	HAL_NVIC_DisableIRQ(bttn_screen_on_EXTI_IRQn);
}

void IdleProcess::Impl::onPwrRelease()
{
//	HAL_NVIC_EnableIRQ(bttn_screen_on_EXTI_IRQn);
}

void IdleProcess::Impl::onPwrClick()
{
	if (sharedData.powerModeState == PowerModeState::Normal) {
		screen.toggle();
	}
}

void IdleProcess::Impl::onPwrHold()
{
	if (sharedData.powerModeState == PowerModeState::Normal) {
		sharedData.powerModeState = PowerModeState::StopRequested;
		screen.off();
	}
}


// -----------------------------------------------------------------
//static void RequestStopMode() { SharedData:: }
// -----------------------------------------------------------------

IdleProcess::~IdleProcess()
{
    delete m_pimpl;
}

void IdleProcess::init()
{
    m_pimpl = new IdleProcess::Impl;
    SharedData::get().setProcessId<ProcessId::Idle>(&m_pimpl->sharedData);

    m_tickRate = 5;
    m_pimpl->screen.on();
}

void IdleProcess::update()
{
    m_pimpl->onTick();
    m_pimpl->handleEvents();

    if (m_pimpl->sharedData.powerModeState == PowerModeState::StopReady)
    {
    	HAL_NVIC_EnableIRQ(bttn_screen_on_EXTI_IRQn);
        EnterStopMode();
        m_pimpl->sharedData.powerModeState = PowerModeState::WakedUp;
        m_pimpl->screen.on();
    }
}



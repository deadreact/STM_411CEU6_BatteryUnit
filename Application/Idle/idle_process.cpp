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
#include "../led.h"
#include "main.h"
#include "../BMS/bms_handler.h"
#include <cstring>
#include <ili9341.h>

// ---------------------------------------------------------------

struct IdleProcess::Impl
{
    void OnTick();
    void HandleEvents();

    void UpdateAnalog();

    ProcessData<ProcessId::Idle> sharedData;

    BMSUpdater m_bmsUpdater;

    LedIndicator led{GPIOC, GPIO_PIN_13, LedIndicationType::Blinking};
    ButtonEventProvider btnEnterSleep{GPIOB, GPIO_PIN_6};
    ButtonEventProvider btnBmsToggle{GPIOA, GPIO_PIN_12, 800, 800};
    ButtonEventHandler btnSleepHandler{&btnEnterSleep, [&]{ sharedData.powerModeState = PowerModeState::StopRequested; }};
    ButtonEventHandler btnScrSwitchHandler{&btnBmsToggle
    , [&]{ led.setIndicationType(LedIndicationType(((int)led.getIndicationType() + 1) % int(LedIndicationType::Count))); }
    , [&]{ sharedData.screenId = (sharedData.screenId + 1) % 3; }
    };
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
    if (sharedData.powerModeState == PowerModeState::StopRequested) {
    	m_bmsUpdater.UpdateAndStop();
        return;
    }

    led.onTick();
    btnEnterSleep.onTick();
    btnBmsToggle.onTick();

    if (sharedData.screenId != 2) {
    	m_bmsUpdater.Update();
    } else {
    	UpdateAnalog();
    }
}

void IdleProcess::Impl::HandleEvents()
{
	btnSleepHandler.handleEvents();
	btnScrSwitchHandler.handleEvents();

    BMSUpdaterEvent bmsEvent = m_bmsUpdater.GetLastEvent();
    if (bmsEvent != BMSUpdaterEvent::NoEvent)
    {
    	sharedData.bms = m_bmsUpdater.GetData();
		sharedData.bmsErrMsg = m_bmsUpdater.debugMsg;
		sharedData.bmsErrFlags = m_bmsUpdater.errFlags;
    }

    if (sharedData.powerModeState == PowerModeState::StopRequested)
    {
    	if (m_bmsUpdater.GetStatus() == BMSStatus::Off)
    	{
    		ILI9341_EnableSleepMode(true);
    		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    		sharedData.powerModeState = PowerModeState::StopReady;
    	}
    }
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
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void IdleProcess::Update()
{
//    bool gotoSleep = m_pimpl->sharedData.sleepingMode > 1;

    m_pimpl->OnTick();
    MX_TouchGFX_Process();
    m_pimpl->HandleEvents();

    if (m_pimpl->sharedData.powerModeState == PowerModeState::StopReady)
    {
        EnterStopMode();
//        m_pimpl->sharedData.sleepingMode = 0;
        m_pimpl->sharedData.powerModeState = PowerModeState::WakedUp;
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        ILI9341_EnableSleepMode(false);
    }
}



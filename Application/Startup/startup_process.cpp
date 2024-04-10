/*
 * startup_process.cpp
 *
 *  Created on: Apr 5, 2024
 *      Author: deadreact
 */

#include "startup_process.h"
#include "power_modes.h"
#include "app_touchgfx.h"
#include "stm32f4xx_hal.h"
#include <shared_data.h>
#include "main.h"

static const int kDuration = 1000;

void StartupProcess::Init()
{
    SharedData::get().setProcessId<ProcessId::Startup>(&data);
    m_startTick = HAL_GetTick();
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void StartupProcess::Deinit()
{
    EnterStandbyMode();
}

void StartupProcess::Update()
{
    data.timeLeftToStandby = kDuration - (HAL_GetTick() - m_startTick);
    MX_TouchGFX_Process();
    m_isRunning = data.timeLeftToStandby > 0;
}

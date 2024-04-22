/*
 * startup_process.cpp
 *
 *  Created on: Apr 5, 2024
 *      Author: deadreact
 */

#include "../startup_process/startup_process.h"

#include <shared_data.h>
#include "power_modes.h"
#include "app_touchgfx.h"
#include "stm32f4xx_hal.h"
#include "main.h"


void StartupProcess::init()
{
    SharedData::get().setProcessId<ProcessId::Startup>(&data);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void StartupProcess::deinit()
{
    EnterStandbyMode();
}

void StartupProcess::update()
{
    data.timeLeftToStandby = m_standbyTimeout.getTimeout() - m_standbyTimeout.getDuration();
    MX_TouchGFX_Process();
    m_isRunning = data.timeLeftToStandby > 0;
}

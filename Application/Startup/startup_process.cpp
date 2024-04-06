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

static const int kDuration = 12000;

void StartupProcess::Init()
{
	SharedData::get().setProcessId<ProcessId::Startup>(&data);

	m_startTick = HAL_GetTick();
}

void StartupProcess::Run()
{
	m_isRunning = true;

	while (m_isRunning)
	{

		data.timeLeftToStandby = kDuration - (HAL_GetTick() - m_startTick);
		MX_TouchGFX_Process();
		m_isRunning = data.timeLeftToStandby > 0;
		HAL_Delay(1);
	}

	EnterStandbyMode();
}

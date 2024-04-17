/*
 * process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "process.h"
#include "stm32f4xx_hal.h"

Process* Process::sm_current = nullptr;

void Process::Run()
{
    if (sm_current) {
        sm_current->m_isRunning = false;
    }

    m_isRunning = true;
    sm_current = this;

    auto updateTick = HAL_GetTick();

    while (m_isRunning)
    {
        if (updateTick <= HAL_GetTick())
        {
            updateTick = HAL_GetTick() + m_tickRate;
            Update();
        }
    }

    sm_current = nullptr;
    Deinit();
}

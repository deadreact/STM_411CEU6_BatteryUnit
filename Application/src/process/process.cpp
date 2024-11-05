/*
 * process.cpp
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#include "process.h"
#include "stm32f4xx_hal.h"
#include <cmsis_os.h>

Process* Process::sm_current = nullptr;

void Process::run()
{
    if (sm_current) {
        sm_current->m_isRunning = false;
    }

    m_isRunning = true;
    sm_current = this;

    while (m_isRunning)
    {
        if (m_tickRate.isReached())
        {
            m_tickRate.reset();
            update();
        }
        else
        {
        	osThreadYield();
        }
    }

    sm_current = nullptr;
    deinit();
}

/*
 * program.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include "program.h"
#include "stm32f4xx_hal.h"
#include "Startup/startup_process.h"
#include "Idle/idle_process.h"


void Program_Process()
{
    /* Check and handle if the system wasn't resumed from Standby mode */
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == RESET)
    {
        StartupProcess p;
        p.Init();
        p.Run();
    }
//    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
//    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

    IdleProcess p;
    p.Init();
    p.Run();
}



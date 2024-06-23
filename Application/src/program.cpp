/*
 * program.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include <program.h>
#include <ili9341.h>
#include "stm32f4xx_hal.h"

#include "process/idle_process.h"

extern "C" {
	extern void touchgfxSignalVSync(void);
	extern TIM_HandleTypeDef htim2;
	extern TIM_HandleTypeDef htim4;
}

void Program_Process()
{
#if 0
    /* Check and handle if the system wasn't resumed from Standby mode */
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == RESET)
    {
        StartupProcess p;
        p.init();
        p.run();
    }
//    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
//    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
#endif
    ILI9341_Init();
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    HAL_TIM_Base_Start_IT(&htim4);

    IdleProcess p;
    p.init();
    p.run();
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        touchgfxSignalVSync();
    }
}

/*
 * power_modes.c
 *
 *  Created on: Apr 6, 2024
 *      Author: deadreact
 */

#include "power_modes.h"
#include "stm32f4xx_hal.h"
#include "main.h"

extern void SystemClock_Config(void);
extern void vTaskSuspendAll(void);
extern long xTaskResumeAll(void);

void EnterSleepMode(void) {
    HAL_SuspendTick();
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    HAL_ResumeTick();
}

void EnterStopMode(void)
{
	HAL_SuspendTick();
    vTaskSuspendAll();
    HAL_GPIO_WritePin(extr_bat_on_GPIO_Port, extr_bat_on_Pin, GPIO_PIN_RESET);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    HAL_GPIO_WritePin(extr_bat_on_GPIO_Port, extr_bat_on_Pin, GPIO_PIN_SET);
    SystemClock_Config();
    HAL_ResumeTick();
    xTaskResumeAll();
}

/**
  * @brief  This function configures the system to enter Standby mode for
  *         current consumption measurement purpose.
  *         STANDBY Mode
  *         ============
  *           - Backup SRAM and RTC OFF
  *           - IWDG and LSI OFF
  *           - Wakeup using WakeUp Pin (PA.00)
  * @param  None
  * @retval None
  */
void EnterStandbyMode(void)
{
  /* Enable Power Clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* Allow access to Backup */
  HAL_PWR_EnableBkUpAccess();

  /* Reset RTC Domain */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();

  /* Disable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

  /* Clear all related wakeup flags */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

  /* Re-enable all used wakeup sources: Pin1(PA.0) */
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);

  /*## Enter Standby Mode ####################################################*/
  /* Request to enter STANDBY mode  */
  HAL_PWR_EnterSTANDBYMode();
}


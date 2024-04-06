/*
 * startup_process.cpp
 *
 *  Created on: Apr 5, 2024
 *      Author: deadreact
 */

#include "startup_process.h"
//#include "main.h"
#include "stm32f4xx_hal.h"
#include "app_touchgfx.h"
#include "stm32f4xx_ll_rcc.h"

static const int kDuration = 12000;

extern int gCurrentProcess;
extern int gTimeLeftToSleep;

void SYSCLKConfig_FromSTOP(void)
{
  /* Customize process using LL interface to improve the performance
     (wake-up time from STOP quicker in LL than HAL)*/
  /* HSE configuration and activation */
  LL_RCC_HSE_Enable();
  while(LL_RCC_HSE_IsReady() != 1) {};

  /* Main PLL activation */
  LL_RCC_PLL_Enable();
  while(LL_RCC_PLL_IsReady() != 1)
  {
  };

  /* SYSCLK activation on the main PLL */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  };
}

void EnableStopMode()
{
	HAL_SuspendTick();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	SYSCLKConfig_FromSTOP();
	HAL_ResumeTick();
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
void EnableStandbyMode()
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

StartupProcess::StartupProcess() {

}

StartupProcess::~StartupProcess() {
}

void StartupProcess::Init()
{
	m_startTick = HAL_GetTick();
}

void StartupProcess::Run()
{
	gCurrentProcess = 1;
	m_isRunning = true;

	while (m_isRunning)
	{
		gTimeLeftToSleep = kDuration - (HAL_GetTick() - m_startTick);
		MX_TouchGFX_Process();
		m_isRunning = gTimeLeftToSleep > 0;
		HAL_Delay(1);
	}

	EnableStandbyMode();
}

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define charg_pot_ud_Pin GPIO_PIN_13
#define charg_pot_ud_GPIO_Port GPIOC
#define charg_pot_inc_Pin GPIO_PIN_14
#define charg_pot_inc_GPIO_Port GPIOC
#define charg_pot_cs_Pin GPIO_PIN_15
#define charg_pot_cs_GPIO_Port GPIOC
#define charger_dcOk_Pin GPIO_PIN_1
#define charger_dcOk_GPIO_Port GPIOA
#define charger_dcOk_EXTI_IRQn EXTI1_IRQn
#define ADC_thermal_Pin GPIO_PIN_4
#define ADC_thermal_GPIO_Port GPIOA
#define SCRN_DC_Pin GPIO_PIN_0
#define SCRN_DC_GPIO_Port GPIOB
#define SCRN_RESET_Pin GPIO_PIN_1
#define SCRN_RESET_GPIO_Port GPIOB
#define SCRN_CS_Pin GPIO_PIN_2
#define SCRN_CS_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_10
#define LED_GPIO_Port GPIOB
#define charger_off_Pin GPIO_PIN_15
#define charger_off_GPIO_Port GPIOB
#define bms_on_Pin GPIO_PIN_8
#define bms_on_GPIO_Port GPIOA
#define bttn_screen_led_Pin GPIO_PIN_11
#define bttn_screen_led_GPIO_Port GPIOA
#define bttn_screen_on_Pin GPIO_PIN_12
#define bttn_screen_on_GPIO_Port GPIOA
#define bttn_screen_on_EXTI_IRQn EXTI15_10_IRQn
#define bttn_usb_led_Pin GPIO_PIN_15
#define bttn_usb_led_GPIO_Port GPIOA
#define bttn_usb_on_Pin GPIO_PIN_3
#define bttn_usb_on_GPIO_Port GPIOB
#define usb_on_Pin GPIO_PIN_4
#define usb_on_GPIO_Port GPIOB
#define bttn_inv_led_Pin GPIO_PIN_5
#define bttn_inv_led_GPIO_Port GPIOB
#define bttn_inv_on_Pin GPIO_PIN_6
#define bttn_inv_on_GPIO_Port GPIOB
#define bms_ok_Pin GPIO_PIN_7
#define bms_ok_GPIO_Port GPIOB
#define bms_ok_EXTI_IRQn EXTI9_5_IRQn
#define inv_ok_Pin GPIO_PIN_8
#define inv_ok_GPIO_Port GPIOB
#define inv_on_Pin GPIO_PIN_9
#define inv_on_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

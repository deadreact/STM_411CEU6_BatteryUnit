/*
 * program.h
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

void Program_Process();

void RTC_Backup(RTC_HandleTypeDef* hrtc, const RTC_AlarmTypeDef* alarmData);
int Check_RTC_Backup(RTC_HandleTypeDef* hrtc);

#ifdef __cplusplus
}
#endif
#endif /* PROGRAM_H_ */

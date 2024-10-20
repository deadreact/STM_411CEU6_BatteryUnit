/*
 * program.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#include <common_defs.h>
#include <program.h>
#include <ili9341.h>
#include "process/idle_process.h"

extern "C" {
    extern void touchgfxSignalVSync(void);

    extern void Error_Handler(void);
    extern TIM_HandleTypeDef htim2;
    extern TIM_HandleTypeDef htim4;
}

static void RestoreAlarm(RTC_HandleTypeDef* hrtc)
{
    const uint32_t alarmVal = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR2);
    if (alarmVal > 0)
    {
        RTC_AlarmTypeDef sAlarm = {0};

        sAlarm.AlarmTime.Hours = (alarmVal % SEC_IN_DAY) / SEC_IN_HOUR;
        sAlarm.AlarmTime.Minutes = (alarmVal % SEC_IN_HOUR) / SEC_IN_MIN;
        sAlarm.AlarmTime.Seconds = alarmVal % SEC_IN_MIN;
        sAlarm.AlarmTime.SubSeconds = 0;
        sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
        sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
        sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
        sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
        sAlarm.AlarmDateWeekDay = alarmVal / SEC_IN_HOUR;
        sAlarm.Alarm = RTC_ALARM_A;

        if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
        {
            Error_Handler();
        }
    }
}

void RTC_Backup(RTC_HandleTypeDef* hrtc, const RTC_AlarmTypeDef* alarmData)
{
    uint32_t data = SEC_IN_DAY * alarmData->AlarmDateWeekDay;
    data += SEC_IN_HOUR * alarmData->AlarmTime.Hours;
    data += SEC_IN_MIN * alarmData->AlarmTime.Minutes;
    data += alarmData->AlarmTime.Seconds;

    HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR1, 0x32f2);
    HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR2, data);
}


int Check_RTC_Backup(RTC_HandleTypeDef* hrtc)
{
#ifdef RELEASE
    // do not set an alarm in Release/Retail
    if (HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR1) == 0x32f2)
    {
        return true;
    }
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    sTime.Hours = 15;
    sTime.Minutes = 14;
    sTime.Seconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
    sDate.Month = RTC_MONTH_APRIL;
    sDate.Date = 24;
    sDate.Year = 24;

    if (HAL_RTC_SetDate(hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR1, 0x32f2);
    return true;
#endif
    if (HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR1) == 0x32f2)
    {
        RestoreAlarm(hrtc);
        return true;
    }

    return false;
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


    IdleProcess p;
    p.init();
    p.run();
}


// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// {
//     if (htim->Instance == TIM4)
//     {
//         touchgfxSignalVSync();
//     }
// }

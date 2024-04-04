/*
 * program.h
 *
 *  Created on: Apr 4, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include "stm32f4xx_hal.h"

struct ProgramDescriptor
{
	volatile uint32_t* brightnessHandle;
	GPIO_TypeDef* btnGPIOx;
	uint16_t btnPin;
};

#ifdef __cplusplus
extern "C" {
#endif
    void Program_Init(const void* desc);
    void Program_Process(void);
#ifdef __cplusplus
}
#endif
#endif /* PROGRAM_H_ */

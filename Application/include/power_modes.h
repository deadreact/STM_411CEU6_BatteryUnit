/*
 * power_modes.h
 *
 *  Created on: Apr 6, 2024
 *      Author: deadreact
 */

#ifndef SRC_POWER_MODES_H_
#define SRC_POWER_MODES_H_

#ifdef __cplusplus
extern "C" {
#endif

void EnterSleepMode(void);
void EnterStopMode(void);
void EnterStandbyMode(void);


#ifdef __cplusplus
}
#endif

#endif /* SRC_POWER_MODES_H_ */

/*
 * common_defs.h
 *
 *  Created on: Oct 20, 2024
 *      Author: deadreact
 */

#ifndef INC_COMMON_DEFS_H_
#define INC_COMMON_DEFS_H_


#define SEC_IN_MIN 60
#define SEC_IN_HOUR (SEC_IN_MIN * 60)
#define SEC_IN_DAY (SEC_IN_HOUR * 24)

enum class ScreenId
{
	DebugScreen = 0,
	MainScreen,
//	ClockScreen,

	Count,
	DefaultScreen = MainScreen
};

enum class ChargeState { Invalid = -1, Idle, Charge, Uncharge };




#endif /* INC_COMMON_DEFS_H_ */

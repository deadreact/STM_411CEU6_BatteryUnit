/*
 * common.h
 *
 *  Created on: Jun 23, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_COMMON_DEFS_H_
#define INCLUDE_COMMON_DEFS_H_

#define SEC_IN_MIN 60
#define SEC_IN_HOUR (SEC_IN_MIN * 60)
#define SEC_IN_DAY (SEC_IN_HOUR * 24)

enum class ScreenId
{
	DebugScreen = 0,
	ClockScreen,
	MainScreen,

	Count,
	DefaultScreen = MainScreen
};

#endif /* INCLUDE_COMMON_DEFS_H_ */

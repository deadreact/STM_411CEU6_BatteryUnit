/*
 * common.h
 *
 *  Created on: Apr 18, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef GUI_INCLUDE_GUI_COMMON_COMMON_H_
#define GUI_INCLUDE_GUI_COMMON_COMMON_H_


enum class ChargeState { Idle, Charge, Uncharge };
constexpr static const ChargeState InvalidChargeState = ChargeState{-1};

#endif /* GUI_INCLUDE_GUI_COMMON_COMMON_H_ */

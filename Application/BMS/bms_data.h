/*
 * bms_data.h
 *
 *  Created on: Apr 8, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef BMS_BMS_DATA_H_
#define BMS_BMS_DATA_H_

#include <stdint.h>

enum class BMSStatus
{
    NoStatus,
    Requested,
    RequestTimedOut,
    Ok,
    InfoTimedOut,
    Error
};

enum class BMSErrorType
{
	NoError,
	RequestError,
	RequestBusy,
	ResponseError,
	ResponseBusy,
	ParseError,
	UARTClbkError
};

struct BatteryData
{
	constexpr static uint8_t kMaxCellCount = 12;
	using voltage_t = uint16_t;

	voltage_t cellVoltage[kMaxCellCount];
    voltage_t voltage{0};
    int16_t current{0};
    uint8_t capacity{0}; // 0 - 100%
    uint8_t cellCount{0};

    // float energyAh{0};

    bool operator==(const BatteryData& other) const;
    inline bool operator!=(const BatteryData& other) const { return !operator==(other); }
};
#endif /* BMS_BMS_DATA_H_ */

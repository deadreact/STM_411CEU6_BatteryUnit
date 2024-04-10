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

struct BMSErrorFlags
{
    constexpr static const uint32_t UART_Parity          = 0x0001;
    constexpr static const uint32_t UART_Noise           = 0x0002;
    constexpr static const uint32_t UART_Frame           = 0x0004;
    constexpr static const uint32_t UART_Overrun         = 0x0008;
    constexpr static const uint32_t UART_DMA             = 0x0010;
    constexpr static const uint32_t UART_Timeout         = 0x0020;
    constexpr static const uint32_t RequestTimeout       = 0x0040;
    constexpr static const uint32_t ParseStartBits       = 0x0080;
    constexpr static const uint32_t ParseCRC             = 0x0100;   // TODO:
    constexpr static const uint32_t ParseValidation      = 0x0200;   // TODO:
    // constexpr static const uint32_t _      = 0x0400;
    // constexpr static const uint32_t _      = 0x0800;
    constexpr static const uint32_t ValidResponseTimeout = 0x1000;

    constexpr static const uint32_t maskMinorErrors = 0x0FFF;
    constexpr static const uint32_t maskMajorErrors = 0xF000;
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

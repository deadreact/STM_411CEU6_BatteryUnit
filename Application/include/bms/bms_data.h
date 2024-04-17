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
    Off,
    OffRequested,
    OnRequested,
    Requested,
    RequestTimedOut,
    Ok,
    InfoTimedOut,
    Error
};

struct BMSErrorFlags
{
    enum : uint32_t
    {
        Ok                   = 0x0000,
        UART_Parity          = 0x0001,
        UART_Noise           = 0x0002,
        UART_Frame           = 0x0004,
        UART_Overrun         = 0x0008,
        UART_DMA             = 0x0010,
        UART_Timeout         = 0x0020,
        RequestTimeout       = 0x0040,
        // _                    = 0x0080,
        ParseControlBytes    = 0x0100,
        ParseChecksum        = 0x0200,
        ParseValidation      = 0x0400,
        ParseUnsupportedType = 0x0800,
        ValidResponseTimeout = 0x1000,

        maskMinorErrors      = 0x0FFF,
        maskMajorErrors      = 0xF000,
        maskParseErrors      = 0x0F00,
        maskErrorDetails     = 0xFFFF0000
    };
};

struct BatteryData
{
    constexpr static uint8_t kMaxCellCount = 12;
    using voltage_t = uint16_t;

    voltage_t cellVoltage[kMaxCellCount];
    voltage_t voltage{0};
    int16_t current{0};
    uint8_t soc{0xff}; // 0 - 100%, invalid
    uint8_t cellCount{0};
    uint32_t capacityAh{0};

    bool isValid() const { return soc <= 100 && capacityAh > 0; }
    int calcTimeRemain(bool invertorOn = false) const;

    bool operator==(const BatteryData& other) const;
    inline bool operator!=(const BatteryData& other) const { return !operator==(other); }
};
#endif /* BMS_BMS_DATA_H_ */

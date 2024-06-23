/*
 * bms_data.h
 *
 *  Created on: Apr 8, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef BMS_BMS_DATA_H_
#define BMS_BMS_DATA_H_

#include <stdint.h>
#include <utils/stack_vector.h>

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
        TurnOnTimeout 		 = 0x2000,
        TurnOffTimeout 		 = 0x4000,

        maskMinorErrors      = 0x0FFF,
        maskMajorErrors      = 0xF000,
        maskParseErrors      = 0x0F00,
        maskErrorDetails     = 0xFFFF0000
    };
};

struct BatteryData
{
    constexpr static uint8_t kMaxCellCount = 12;

    utils::stack_vector<uint16_t, kMaxCellCount> cellVoltage;

    uint16_t voltage{0};
    int16_t current{0};
    uint8_t soc{0xff}; // 0 - 100%, invalid
    uint32_t capacityAh{0};
    int8_t battery_box_temperature{0};
    int8_t battery_temperature{0};

    uint16_t warningMsg{0};
    uint16_t status{0};

    bool isValid() const { return (isLowCapacity() || soc > 0) && soc <= 100 && capacityAh > 0; }
    int calcTimeRemain(int16_t curr) const;

    bool operator==(const BatteryData& other) const;
    inline bool operator!=(const BatteryData& other) const { return !operator==(other); }

    // --------------------- warning -------------------------
    inline bool isLowCapacity() const { return warningMsg & 1; }
};

struct SmoothedValue
{
	constexpr static const int kBufferSize = 5;
	int16_t values[kBufferSize];

	SmoothedValue();

	void reset();
	void set(int16_t value);
	int16_t get() const;
};

#endif /* BMS_BMS_DATA_H_ */

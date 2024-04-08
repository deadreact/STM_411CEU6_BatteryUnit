/*
 * bms_handler.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef BMS_BMS_HANDLER_H_
#define BMS_BMS_HANDLER_H_

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

struct BatteryData
{
    uint16_t* cellVoltage{nullptr};
    uint16_t cellCount{0};

    uint16_t voltage{0};
    uint16_t current{0};
    uint16_t capacity{0};
    float energyAh{0};

    BatteryData() = default;
    BatteryData(const BatteryData&);
    BatteryData& operator=(const BatteryData&);
    BatteryData(BatteryData&&);
    BatteryData& operator=(BatteryData&&);
    ~BatteryData();

    void setCellCount(uint16_t count);

    bool operator==(const BatteryData& other) const;
    inline bool operator!=(const BatteryData& other) const { return !operator==(other); }
};

class BMSHandler
{
public:
    BMSHandler();
    ~BMSHandler();

    void Request();
//private:
    void ParseData(uint8_t *rawData);
    void UpdateData(BatteryData&& newData);
private:
    BMSStatus m_status { BMSStatus::NoStatus };
    BatteryData m_data;
};

#endif /* BMS_BMS_HANDLER_H_ */

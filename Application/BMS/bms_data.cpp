/*
 * bms_data.cpp
 *
 *  Created on: Apr 8, 2024
 *      Author: Dmitriy.Gyr
 */

#include "bms_data.h"
#include <cstdlib>
#include <cstring>

BatteryData::BatteryData(const BatteryData& other)
    : voltage(other.voltage)
    , current(other.current)
    , capacity(other.capacity)
    // , energyAh(other.energyAh)
{
    setCellCount(other.cellCount);
    memcpy(cellVoltage, other.cellVoltage, sizeof(voltage_t)*cellCount);
}

BatteryData& BatteryData::operator=(const BatteryData& other)
{
    setCellCount(other.cellCount);
    memcpy(cellVoltage, other.cellVoltage, sizeof(voltage_t)*cellCount);
    voltage = other.voltage;
    current = other.current;
    capacity = other.capacity;
    // energyAh = other.energyAh;

    return *this;
}

BatteryData::BatteryData(BatteryData&& other)
    : cellVoltage(other.cellVoltage)
    , voltage(other.voltage)
    , current(other.current)
    , capacity(other.capacity)
    , cellCount(other.cellCount)
    // , energyAh(other.energyAh)
{
    other.cellVoltage = nullptr;
    other.cellCount = 0;
}

BatteryData& BatteryData::operator=(BatteryData&& other)
{
    cellVoltage = other.cellVoltage;
    voltage = other.voltage;
    current = other.current;
    capacity = other.capacity;
    cellCount = other.cellCount;
    // energyAh = other.energyAh;

    other.cellVoltage = nullptr;
    other.cellCount = 0;

    return *this;
}

BatteryData::~BatteryData()
{
    if (cellVoltage != nullptr)
    {
        free(cellVoltage);
        cellVoltage = nullptr;
        cellCount = 0;
    }
}

void BatteryData::setCellCount(uint8_t count)
{
    if (count != cellCount)
    {
        if (cellVoltage == nullptr)
        {
        	cellVoltage = (voltage_t*)malloc(sizeof(voltage_t)*count);
            if (cellVoltage != nullptr) {
            	cellCount = count;
            }
        }
        else if (voltage_t* ptr = (voltage_t*)realloc(cellVoltage, sizeof(voltage_t)*count))
        {
            cellVoltage = ptr;
            cellCount = count;
        }
    }
}

bool BatteryData::operator==(const BatteryData& other) const
{
    if (cellCount == other.cellCount) {
        for (uint8_t cell = 0; cell < cellCount; ++cell) {
            if (cellVoltage[cell] != other.cellVoltage[cell]) {
                return false;
            }
        }
    }
    return voltage == other.voltage && current == other.current && capacity == other.capacity /*&& energyAh == other.energyAh*/;
}

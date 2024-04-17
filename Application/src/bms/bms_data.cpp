/*
 * bms_data.cpp
 *
 *  Created on: Apr 8, 2024
 *      Author: Dmitriy.Gyr
 */

#include <bms/bms_data.h>
#include <cstdlib>
#include <cstring>

bool BatteryData::operator==(const BatteryData& other) const
{
    return cellCount == other.cellCount
        && (cellCount == 0 || memcmp(cellVoltage, other.cellVoltage, sizeof(voltage_t)*cellCount) == 0)
        && voltage == other.voltage
        && current == other.current
        && soc == other.soc
        && capacityAh == other.capacityAh
        /*&& energyAh == other.energyAh*/;
}

int BatteryData::calcTimeRemain(bool invertorOn) const
{
     /*
    Time
    current < 0: (soc * capacityAh)/current
    current > 0:
        inv on:  ((1 - soc) * capacityAh)/current
        inv off:
        if (soc <= 0.8)
        {
            (capacityAh * (0.8 - soc))/current + (capacityAh * 0.2)/(current * 0.5)
        }
        else
        {
            ((1 - soc) * capacityAh)/current
        }
      */

    if (current < 0)
    {
        return int(soc * capacityAh * 60) / current; // minutes
    }
    else if (current > 0)
    {
        if (invertorOn)
        {
            return int((100 - soc) * capacityAh * 60)/current;
        }
        else if (soc <= 80)
        {
            return int(capacityAh * (80 - soc) * 60)/current + (capacityAh * 20 * 60)/(current * 0.5f);
        }
        else
        {
            return int((100 - soc) * capacityAh * 60)/current;
        }
    }
    return 0;
}


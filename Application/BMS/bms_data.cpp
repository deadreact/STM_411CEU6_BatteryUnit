/*
 * bms_data.cpp
 *
 *  Created on: Apr 8, 2024
 *      Author: Dmitriy.Gyr
 */

#include "bms_data.h"
#include <cstdlib>
#include <cstring>

bool BatteryData::operator==(const BatteryData& other) const
{
    return cellCount == other.cellCount
    	&& (cellCount == 0 || memcmp(cellVoltage, other.cellVoltage, sizeof(voltage_t)*cellCount) == 0)
		&& voltage == other.voltage
		&& current == other.current
		&& capacity == other.capacity
		/*&& energyAh == other.energyAh*/;
}

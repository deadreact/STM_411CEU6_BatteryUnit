/*
 * bms_data.cpp
 *
 *  Created on: Apr 8, 2024
 *      Author: Dmitriy.Gyr
 */

#include <bms_data.h>
#include <cstdlib>
#include <cstring>

bool BatteryData::operator==(const BatteryData& other) const
{
    return cellVoltage == other.cellVoltage
        && voltage == other.voltage
        && current == other.current
        && soc == other.soc
        && capacityAh == other.capacityAh
        && battery_box_temperature == other.battery_box_temperature
        && battery_temperature == other.battery_temperature
        && warningMsg == other.warningMsg
        && status == other.status
        /*&& energyAh == other.energyAh*/;
}

int BatteryData::calcTimeRemain(int16_t curr) const
{
    if (curr < 0)
    {
        return int(soc * capacityAh * 60 * 60) / curr; // seconds
    }
    else if (curr > 0)
    {
    	return int((100 - soc) * capacityAh * 60 * 60)/curr; // seconds
//        if (invertorOn)
//        {
//            return int((100 - soc) * capacityAh * 60 * 60)/curr;
//        }
//        else if (soc <= 80)
//        {
//            return int(capacityAh * (80 - soc) * 60 * 60)/curr + (capacityAh * 20 * 60 * 60)/(curr * 0.5f);
//        }
//        else
//        {
//            return int((100 - soc) * capacityAh * 60 * 60)/curr;
//        }
    }
    return 0;
}


SmoothedValue::SmoothedValue()
{
	reset();
}

void SmoothedValue::reset()
{
	memset(values, 0, sizeof(values));
}

void SmoothedValue::set(int16_t value)
{
	for (int16_t* ptr = values + kBufferSize - 1; ptr != values; --ptr) {
		*ptr = *(ptr - 1);
	}

	values[0] = value;

}

int16_t SmoothedValue::get() const
{
	constexpr float koef[kBufferSize] = {0.25f, 0.25f, 0.2f, 0.2f, 0.1f};

	float result = 0.f;
	for (int i = 0; i < kBufferSize; ++i) {
		result += values[i] * koef[i];
	}

	return result;
}


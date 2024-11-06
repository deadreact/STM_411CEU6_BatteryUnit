/*
 * ntchandler.cpp
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#include <ntc_table.h>
#include <handlers/ntc_handler.h>
#include <cmath>
#include <array>

extern ADC_HandleTypeDef hadc1;


//const uint8_t linear_power_table[]     = {10,14,18,22,27,33,40,46,57,74,89,92,93,94,95,96,97,98,99,100};
//const int16_t linear_temperature_table[] = {32,34,35,37,38,39,40,44,45,46,48,49,50,52,53,54,55,56,58,60};
//const uint8_t linear_power_table[]     = {1,2,5,13,27,42,69,94,100};
//const uint8_t linear_temperature_table[] = {27,28,29,30,31,32,33, 35, 37};

const uint16_t linear_power_table[]     = {562,585,608,631,654,677,700,723,746,769,792,815,838,861,884,907,930,953,976,999}; // linear step 23, PWM set to TIM2->CCR2
const int16_t linear_temperature_table[] = {32,34,35,37,38,39,40,44,45,46,48,49,50,52,53,54,55,56,58,60};

// PWM 0   -> 2.049v
// PWM 100 -> 1.926v
// PWM 200 -> 1.820v
// PWM 300 -> 1.696v
// PWM 400 -> 1.515v // turn off value
// PWM 500 -> 3.054v
// PWM 560 -> 4.091v // start value
// PWM 600 -> 4.942v
// PWM 700 -> 6.810v
// PWM 800 -> 8.670v
// PWM 900 -> 10.52v
// PWM 999 -> 12.06v


 std::map<int16_t, uint16_t> make_map()
 {
 	std::map<int16_t, uint16_t> map;

 	for (size_t i = 0; i < std::size(linear_power_table); i++)
 	{
 		map[linear_temperature_table[i]] = linear_power_table[i];
 	}
 	return map;
 }

NtcHandler::NtcHandler(uint32_t adcChannel1, uint32_t adcChannel2)
	: m_adcChannel1(adcChannel1)
	, m_adcChannel2(adcChannel2)
	 , m_table(make_map())
{
    m_adcConfig.Rank = 1;
    m_adcConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    m_adcConfig.Channel = adcChannel1;
}

void NtcHandler::onTick()
{
	if (m_updateTimeout.isReached())
	{
		m_updateTimeout.reset();
	    int adcValue = readSensor(m_adcChannel1);
	    if (adcValue >= 0)
		{
			m_temperature1 = ntc::calc_temperature(adcValue) / 10;
		}

	    adcValue = readSensor(m_adcChannel2);
	    if (adcValue >= 0)
		{
			m_temperature2 = ntc::calc_temperature(adcValue) / 10;
		}

	    auto temperature = std::max(m_temperature1, m_temperature2);

	    auto fanValue = calcFanValue(temperature);
		if (m_fanValue != fanValue)
		{
			m_fanValue = fanValue;
		}
	}
}

int NtcHandler::readSensor(uint32_t adcChannel)
{
    if (m_adcConfig.Channel != adcChannel)
    {
        m_adcConfig.Channel = adcChannel;
        if (HAL_ADC_ConfigChannel(&hadc1, &m_adcConfig) != HAL_OK)
        {
            return -1;
        }
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);

    const int value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return value;
}

uint16_t NtcHandler::calcFanValue(int16_t temperature) const
{
	 auto it = m_table.upper_bound(temperature);
	 if (it == m_table.end())
	 {
	 	return m_table.rbegin()->second;
	 }
	 else if (it == m_table.begin())
	 {
	 	return 0;
	 }
	 return std::prev(it)->second;
//
//	 if (temperature < linear_temperature_table[0]) {
//	 	return 0;
//	 }
//	for (int i = std::ssize(linear_temperature_table) - 1; i >= 0; i--)
//	{
//		if (temperature >= linear_temperature_table[i])
//		{
//			return linear_power_table[i];
//		}
//	}
//	return 0;
}

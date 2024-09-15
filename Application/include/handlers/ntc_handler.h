/*
 * ntchandler.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_HANDLERS_NTC_HANDLER_H_
#define INCLUDE_HANDLERS_NTC_HANDLER_H_

#include <main.h> // GPIO defines & HAL
#include <utils/timeout.h>
#include <map>

//TODO: multiple channels
class NtcHandler
{
public:
	NtcHandler(uint32_t adcChannel);

	void onTick();
	uint8_t getFanValue() const { return m_fanValue; }
	uint8_t getTemperature() const { return m_temperature; }
private:
	int readSensor(uint32_t adcChannel);

	uint8_t calcFanValue() const;
private:
	ADC_ChannelConfTypeDef m_adcConfig;
	uint8_t m_temperature{0};
	uint8_t m_fanValue{0};

	CTimeout m_updateTimeout{1000};
	const std::map<uint8_t, uint8_t> m_table;
};

#endif /* INCLUDE_HANDLERS_NTC_HANDLER_H_ */

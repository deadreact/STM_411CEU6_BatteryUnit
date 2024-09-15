/*
 * ntchandler.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_HANDLERS_NTC_HANDLER_H_
#define INCLUDE_HANDLERS_NTC_HANDLER_H_

#include <main.h> // GPIO defines & HAL
#include <gpio_wrappers/pin_wrapper.h>
#include <utils/timeout.h>
#include <map>

enum class NtcEvent : uint8_t
{
    NoEvent,
    Updated
};

//TODO: multiple channels
class NtcHandler
{
public:
	NtcHandler(uint32_t adcChannel);

	void update();
	NtcEvent takeLastEvent();
	uint8_t getFanValue() const;
	uint8_t getTemperature() const { return m_temperature; }
private:
	int readSensor(uint32_t adcChannel);
	bool shouldReadSensors() const;

	uint8_t calcFanValue() const;
private:
	ADC_ChannelConfTypeDef m_adcConfig;
	NtcEvent m_lastEvent{NtcEvent::NoEvent};
	uint8_t m_temperature{0};
	uint8_t m_fanValue{0};

	const PinWrapper m_chargerDcOkPin{charger_dcOk_GPIO_Port, charger_dcOk_Pin};
	const PinWrapper m_invOk{inv_ok_GPIO_Port, inv_ok_Pin};

	CTimeout m_updateTimeout{1000};
	const std::map<uint8_t, uint8_t> m_table;
};

#endif /* INCLUDE_HANDLERS_NTC_HANDLER_H_ */

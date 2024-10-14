/*
 * fan_handler.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_HANDLERS_FAN_HANDLER_H_
#define INCLUDE_HANDLERS_FAN_HANDLER_H_


#include <main.h> // GPIO defines & HAL
#include <gpio_wrappers/digital_potentiometer.h>
#include <handlers/ntc_handler.h>


//TODO: multiple channels
class FanHandler
{
public:
	FanHandler();

	void update();

	uint8_t getTemperature1() const { return m_ntcHandler.getTemperature1(); }
	uint8_t getTemperature2() const { return m_ntcHandler.getTemperature2(); }
	uint8_t getFan() const { return m_potCS1.getValue(); }
private:
	void updatePot(uint8_t fanValue);
	bool isEnabled() const;
private:
	Timeout m_fanExtraTime{0};

	NtcHandler m_ntcHandler{ADC_CHANNEL_4, ADC_CHANNEL_6};

	const PinWrapper m_chargerDcOkPin{charger_dcOk_GPIO_Port, charger_dcOk_Pin};
	const PinWrapper m_invOk{inv_ok_GPIO_Port, inv_ok_Pin};
	const PinWrapper m_usbOn{usb_on_GPIO_Port, usb_on_Pin};

	DigitalPotentiometer m_potCS1{pot_CS1_GPIO_Port, pot_CS1_Pin, true};

	PinWrapper m_fanOn{fan_n_power_on_GPIO_Port, fan_n_power_on_Pin};
};
#endif /* INCLUDE_HANDLERS_FAN_HANDLER_H_ */

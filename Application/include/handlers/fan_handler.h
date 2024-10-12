/*
 * fan_handler.h
 *
 *  Created on: Sep 15, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef INCLUDE_HANDLERS_FAN_HANDLER_H_
#define INCLUDE_HANDLERS_FAN_HANDLER_H_


#include <main.h> // GPIO defines & HAL
#include <gpio_wrappers/pin_wrapper.h>
#include <handlers/ntc_handler.h>
#include <utils/timeout.h>

class DigitalPotentiometer
{
public:
	DigitalPotentiometer(GPIO_TypeDef* chipSelect_GPIOx, uint16_t chipSelect_GPIO_Pin)
		: m_potCS(chipSelect_GPIOx, chipSelect_GPIO_Pin)
	{
		m_changeValueDelay.setPaused(true);
	}

	void setValue(uint8_t value);
	uint8_t getValue() const { return m_currentValue; }

private:
	void changeValue(bool increase);	

	PinWrapper m_potCS;
	PinWrapper m_potINC{pot_INC_GPIO_Port, pot_INC_Pin};
	PinWrapper m_potUD{pot_UD_GPIO_Port, pot_UD_Pin};

	uint8_t m_currentValue{100};
	CTimeout m_changeValueDelay{10};
};

//TODO: multiple channels
class FanHandler
{
public:
	FanHandler();

	void update();

	uint8_t getTemperature() const { return m_ntcHandler.getTemperature(); }
	uint8_t getFan() const { return m_potCS1.getValue(); }
private:
	void updatePot(uint8_t fanValue);
	bool isEnabled() const;
private:
	// CTimeout m_changeValueDelay{10};
	Timeout m_fanExtraTime{0};

	NtcHandler m_ntcHandler{ADC_CHANNEL_4};

	const PinWrapper m_chargerDcOkPin{charger_dcOk_GPIO_Port, charger_dcOk_Pin};
	const PinWrapper m_invOk{inv_ok_GPIO_Port, inv_ok_Pin};
	const PinWrapper m_usbOn{usb_on_GPIO_Port, usb_on_Pin};

	DigitalPotentiometer m_potCS1{pot_CS1_GPIO_Port, pot_CS1_Pin};

	PinWrapper m_fanOn{fan_n_power_on_GPIO_Port, fan_n_power_on_Pin};
};
#endif /* INCLUDE_HANDLERS_FAN_HANDLER_H_ */

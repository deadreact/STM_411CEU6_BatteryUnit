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

//TODO: multiple channels
class FanHandler
{
public:
	void update();

	uint8_t getTemperature() const { return m_ntcHandler.getTemperature(); }
	uint8_t getFan() const { return m_currentFanValue; }
private:
	void changeValue(bool increase);
	void updatePot();
	void X9C_decrease();
	void X9C_increase();
	void X9C_store();
	void setDigipotVal(uint8_t val);
private:
	uint8_t m_currentFanValue{100};
	uint8_t m_fanValue{0};
	CTimeout m_delay{10};

	NtcHandler m_ntcHandler{ADC_CHANNEL_4};
	const PinWrapper m_chargerDcOkPin{charger_dcOk_GPIO_Port, charger_dcOk_Pin};
	const PinWrapper m_invOk{inv_ok_GPIO_Port, inv_ok_Pin};

	PinWrapper m_potINC{pot_INC_GPIO_Port, pot_INC_Pin};
	PinWrapper m_potUD{pot_UD_GPIO_Port, pot_UD_Pin};
	PinWrapper m_potCS1{pot_CS1_GPIO_Port, pot_CS1_Pin};

	PinWrapper m_fanOn{fan_n_power_on_GPIO_Port, fan_n_power_on_Pin};
};
#endif /* INCLUDE_HANDLERS_FAN_HANDLER_H_ */

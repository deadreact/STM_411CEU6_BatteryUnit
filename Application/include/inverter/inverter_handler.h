/*
 * invertor_handler.h
 *
 *  Created on: Apr 19, 2024
 *      Author: Dmitriy.Gyr
 */

#ifndef SRC_INVERTER_INVERTER_HANDLER_H_
#define SRC_INVERTER_INVERTER_HANDLER_H_

#include <gpio_wrappers/button.h>
#include <gpio_wrappers/led.h>
#include <gpio_wrappers/io_tube.h>
#include <application_utils.h>
#include <main.h> // GPIO defines & HAL

enum class InverterState
{
	Off = int(LedIndicationType::Off),
	On = int(LedIndicationType::On),
	Intermediate = int(LedIndicationType::FastBlinking)
};

class InverterHandler
{
public:
	InverterHandler();
	
	void onTick();
	void handleEvents();

	void setActive(bool active);
	inline InverterState getState() const { return InverterState(m_led.getIndicationType()); }
private:
	void onBtnClicked();
private:
	ButtonEventProvider m_btn{bttn_inv_on_GPIO_Port, bttn_inv_on_Pin};
	LedIndicator m_led{bttn_inv_led_GPIO_Port, bttn_inv_led_Pin};
	const SinglePinElement m_invOk{inv_ok_GPIO_Port, inv_ok_Pin};
	SinglePinElement m_invOn{inv_on_GPIO_Port, inv_on_Pin};

	ButtonEventHandler m_btnHandler{&m_btn, [&]{ onBtnClicked(); }};
	
	CTimeout m_stableStateCheckTimeout{2200};
	CTimeout m_forceTurnOffTimeout{8000};
	bool m_isActive{true};

//	bool m_isOn{false};
};

#endif /* SRC_INVERTER_INVERTER_HANDLER_H_ */

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
#include <main.h> // GPIO defines & HAL

class InverterHandler
{
public:
	InverterHandler();
	
	void onTick();
	void handleEvents();
private:
	ButtonEventProvider m_btn{bttn_inv_on_GPIO_Port, bttn_inv_on_Pin};
	LedIndicator m_led{bttn_inv_led_GPIO_Port, bttn_inv_led_Pin};
	SinglePinElement m_invFeedback{inv_ok_GPIO_Port, inv_ok_Pin};
	SinglePinElement m_invOn{inv_on_GPIO_Port, inv_on_Pin};

	ButtonEventHandler m_btnHandler{&m_btn, [&]{ m_invOn.togglePin(); }};
	
	uint32_t m_lastOnFeedbackTick{0};
	uint32_t m_lastOffFeedbackTick{0};
	uint32_t m_stableFeedbackDuration{0};
};

#endif /* SRC_INVERTER_INVERTER_HANDLER_H_ */

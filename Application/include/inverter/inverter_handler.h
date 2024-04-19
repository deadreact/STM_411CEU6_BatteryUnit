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

	inline InverterState getState() const { return InverterState(m_led.getIndicationType()); }
private:
	void onBtnClicked();
private:
	ButtonEventProvider m_btn{bttn_inv_on_GPIO_Port, bttn_inv_on_Pin};
	LedIndicator m_led{bttn_inv_led_GPIO_Port, bttn_inv_led_Pin};
	SinglePinElement m_invFeedback{inv_ok_GPIO_Port, inv_ok_Pin};
	SinglePinElement m_invOn{inv_on_GPIO_Port, inv_on_Pin};
//	IOTube m_tube{m_invFeedback, m_led};

	ButtonEventHandler m_btnHandler{&m_btn, [&]{ onBtnClicked(); }};
	
//	uint32_t m_lastOnFeedbackTick{0};
//	uint32_t m_lastOffFeedbackTick{0};
//	uint32_t m_stableFeedbackDuration{0};
	uint32_t m_stableStateCheckTick{0};
	uint32_t m_forceTurnOffTick{0};

//	bool m_isOn{false};
};

#endif /* SRC_INVERTER_INVERTER_HANDLER_H_ */

/*
 * safety_controller.h
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#ifndef INCLUDE_HANDLERS_CHARGER_HANDLER_H_
#define INCLUDE_HANDLERS_CHARGER_HANDLER_H_

#include <gpio_wrappers/pin_wrapper.h>
#include <main.h>
#include <utils/timeout.h>
//#include <gpio_wrappers/digital_potentiometer.h>

struct BatteryData;

enum class ChargerError: uint32_t
{
	NoError 		 = 0,
	BMSError 		 = 0x01000000,
	OvervoltageError = 0x02000000
};

enum class ChargerState { Idle, Investigation, Error };

class ChargerHandler
{
public:
	ChargerHandler();

	inline ChargerState getState() const { return m_state; }
	inline void resetState() { changeState(ChargerState::Idle); }
	bool isDcOk() const { return m_chargerDcOkPin.readPin() == GPIO_PIN_RESET; }

	void update();
private:
	void updateIdle();
	void updateInvestigation();
	void updateError();

	void changeState(ChargerState state, uint32_t flags = 0);
	void analyzeBMSData(const BatteryData& data);

	void setChargePower(uint16_t watts);
private:
	PinWrapper m_chargerOnPin{charger_on_GPIO_Port, charger_on_Pin};
	const PinWrapper m_chargerDcOkPin{charger_dcOk_GPIO_Port, charger_dcOk_Pin};

//	AsyncDigitalPotentiometer m_potCS2{pot_CS2_GPIO_Port, pot_CS2_Pin, false};

	uint32_t m_errFlags{0};
	uint32_t m_bmsDataRevision{0};
	ChargerState m_state{ChargerState::Idle};

	static const uint16_t kOvervoltageSingleValue{3900};
	CTimeout m_investigationTimeout{10000};
	CTimeout m_overvoltageTimeout{60 * 1000}; // 1 min
};

#endif /* INCLUDE_HANDLERS_CHARGER_HANDLER_H_ */

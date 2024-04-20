/*
 * safety_controller.h
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#ifndef SRC_CHARGER_HANDLER_H_
#define SRC_CHARGER_HANDLER_H_

#include <gpio_wrappers/interface.h>
#include <main.h>

struct BatteryData;

enum class ChargerHandlerEvent
{
	NoEvent,
	BMSTurnOffNeeded,
	BMSTurnOnNeeded,
	ChargeError
};

enum class ChargerError
{
	NoError,
	BMSError,
	OvervoltageError
};

class ChargerHandler
{
public:
	enum class State { Idle, Investigation, Error };
public:
	ChargerHandler();

	void update();

	ChargerHandlerEvent takeLastEvent();
private:
	template <State>
	void updateState();

	void changeState(State state, uint32_t flags = 0);
	void analyzeBMSData(const BatteryData& data);
private:
	SinglePinElement m_chargerOffPin{charger_off_GPIO_Port, charger_off_Pin};
	const SinglePinElement m_chargerDcOkPin{charger_dcOk_GPIO_Port, charger_dcOk_Pin};
	const SinglePinElement m_bmsOkStatus{bms_ok_GPIO_Port, bms_ok_Pin};

	uint32_t m_tickStartInvestigation{0xffffffff};
	uint32_t m_errFlags{0};
	uint32_t m_bmsDataRevision{0};
	State m_state{State::Idle};
	ChargerHandlerEvent m_lastEvent{ChargerHandlerEvent::NoEvent};

	uint16_t m_overvoltageNoErrorTick{0};
	static const uint16_t kOvervoltageSingleValue{3900};
	static const uint16_t kOvervoltageErrorTheshold{60 * 1000}; // 1 min
};

#endif /* SRC_CHARGER_HANDLER_H_ */

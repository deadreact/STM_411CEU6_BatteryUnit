/*
 * safety_controller.cpp
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#include "charger_handler.h"

#include <shared_data.h>

template <>
void ChargerHandler::updateState<ChargerHandler::State::Idle>()
{
	if (m_chargerDcOkPin.readPin())
	{
		if (SharedData::getProcessId() == ProcessId::Idle)
		{
			const auto& data = SharedData::getData<ProcessId::Idle>();

			uint32_t majorErrors = data.bmsErrFlags & BMSErrorFlags::maskMajorErrors;
			if (majorErrors)
			{
				changeState(State::Investigation, majorErrors);
			}
			else
			{
				if (m_bmsDataRevision < data.bms.getDataRevision()) {
					m_bmsDataRevision = data.bms.getDataRevision();
					analyzeBMSData(data.bms);
				}
			}
		}
	}
}

template <>
void ChargerHandler::updateState<ChargerHandler::State::Investigation>()
{
	const auto currentTick = HAL_GetTick();
	const auto& data = SharedData::getData<ProcessId::Idle>();
	const auto majorErrors = (data.bmsErrFlags & BMSErrorFlags::maskMajorErrors);

	if (m_tickStartInvestigation > currentTick) {
		if (m_errFlags != majorErrors)
		{
			m_errFlags = majorErrors;
			if (m_errFlags) {
				m_tickStartInvestigation = currentTick + 5000;
			} else {
				changeState(State::Idle);
			}
		}
		return;
	}

	m_errFlags |= majorErrors;

	// TurnOffTimeout: Вирубаємо?
	if (m_errFlags & BMSErrorFlags::TurnOffTimeout)
	{
		changeState(State::Error, BMSErrorFlags::TurnOffTimeout);
	}
	// TurnOnTimeout: пробуємо request ?
	else if (m_errFlags & BMSErrorFlags::TurnOnTimeout)
	{
		changeState(State::Error, BMSErrorFlags::TurnOnTimeout);
	}
	// ValidResponseTimeout: пробуємо вимк/увімк/чекаємо респонс
	else if (m_errFlags & BMSErrorFlags::ValidResponseTimeout)
	{
		m_chargerOffPin.writePin(GPIO_PIN_SET);
		uint8_t step = m_errFlags & 0xf;

		if (step == 0)
		{
			m_errFlags = (m_errFlags & 0xfffffff0) | 1;
			m_lastEvent = ChargerHandlerEvent::BMSTurnOffNeeded;
			// request to turn off bms
		}
		else if (step == 1)
		{
			if (!m_bmsOkStatus.readPin()) // Turned off
			{
				m_errFlags = (m_errFlags & 0xfffffff0) | 2;
				m_lastEvent = ChargerHandlerEvent::BMSTurnOnNeeded;
				// request to turn on
			}
		}
		else if (step == 2)
		{
			if (m_bmsOkStatus.readPin()) // Turned on
			{
				m_errFlags = (m_errFlags & 0xfffffff0) | 3;
				// request data
			}
		}
		else
		{
			if (majorErrors) {
				changeState(State::Error, BMSErrorFlags::ValidResponseTimeout);
			} else if (m_bmsDataRevision < data.bms.getDataRevision()) {
				changeState(State::Idle, 0);
			}
		}
	}
	// Внутрішній аналіз
	else
	{
//		if (m_bmsDataRevision < data.bms.getDataRevision()) {
//			analyzeBMSData(data.bms);
//		}

		changeState(State::Error, m_errFlags);
	}
}

template <>
void ChargerHandler::updateState<ChargerHandler::State::Error>()
{
	m_chargerOffPin.writePin(GPIO_PIN_SET);
}


ChargerHandler::ChargerHandler()
{}

ChargerHandlerEvent ChargerHandler::takeLastEvent()
{
	auto tmp = m_lastEvent;
	m_lastEvent = ChargerHandlerEvent::NoEvent;
	return tmp;
}

void ChargerHandler::update()
{
	switch (m_state)
	{
	case State::Idle:
		updateState<State::Idle>();
		break;
	case State::Investigation:
		updateState<State::Investigation>();
		break;
	case State::Error:
		updateState<State::Error>();
		break;
	}
}


void ChargerHandler::changeState(State state, uint32_t flags)
{
	if (m_state != state)
	{
		if (state == State::Idle)
		{
			m_chargerOffPin.writePin(GPIO_PIN_RESET);
		}
		else if (state == State::Investigation)
		{
			// 1. 5 сек чекаємо, може роздуплиться
			m_tickStartInvestigation = HAL_GetTick() + 5000;
			m_errFlags = flags;
		}
		else if (state == State::Error)
		{
			m_chargerOffPin.writePin(GPIO_PIN_SET);
			m_lastEvent = ChargerHandlerEvent::ChargeError;
		}

		m_state = state;
	}
}


void ChargerHandler::analyzeBMSData(const BatteryData& data)
{
	const auto currentTick = HAL_GetTick();
	/* Handle BMS warn msg
	Bit 0: low capacity                   (1)
	Bit 1: MOS tube overtemperature       (2)
	Bit 2: charging overvoltage           (4)
	Bit 3: discharge undervoltage         (8)
	Bit 4: battery over temperature       (16)
	Bit 5: charging overcurrent           (32)
	Bit 6: discharge overcurrent          (64)
	Bit 7: cell differential pressure     (128)
	Bit 8: overtemperature in battery box (256)
	Bit 9: battery low temperature        (512)
	Bit 10: monomer overvoltage           (1024)
	Bit 11: monomer undervoltage          (2048)
	Bit 12: 309_ A protection 1           (4096)
	Bit 13: 309_ B protection 1           (8192)
	*/
//	static const uint16_t mask = 0x3ffe;
//	static const uint16_t temperature_mask = (1 << 1) | (1 << 4) | (1 << 8) | (1 << 9);
//	const auto warnMsg = data.warningMsg;

	bool isOvervoltage{false};
	for (const auto& voltage: data.cellVoltage) {
		isOvervoltage |= voltage > kOvervoltageSingleValue;
	}

	if (!isOvervoltage) {
		m_overvoltageNoErrorTick = currentTick;
	} else if (m_overvoltageNoErrorTick < currentTick - kOvervoltageErrorTheshold) {
		changeState(State::Error);
	}
}


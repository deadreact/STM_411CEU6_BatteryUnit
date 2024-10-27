/*
 * safety_controller.cpp
 *
 *  Created on: Apr 20, 2024
 *      Author: deadreact
 */

#include <handlers/charger_handler.h>
#include <shared_data.h>

/*

 21 - 250Вт
42 - 500Вт
62 - 750Вт
84 - 1000Вт
100 - 1200Вт

 */

ChargerHandler::ChargerHandler()
{
	const auto& data = *SharedData::getData();
	setChargePower(data.settings.getValue(SettingsData::ChargePower));
}

void ChargerHandler::update()
{
	const auto& data = *SharedData::getData();

	if (data.settings.active)
	{
		setChargePower(data.settings.getValue(SettingsData::ChargePower));
	}

	m_potCS2.onTick();

	switch (m_state)
	{
	case ChargerState::Idle:
		updateIdle();
		break;
	case ChargerState::Investigation:
		updateInvestigation();
		break;
	case ChargerState::Error:
		updateError();
		break;
	}
}

void ChargerHandler::updateIdle()
{
	if (isDcOk())
	{
		const auto& data = *SharedData::getData();

		uint32_t majorErrors = data.errFlags & BMSErrorFlags::maskMajorErrors;
		if (majorErrors)
		{
			changeState(ChargerState::Investigation, majorErrors);
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

void ChargerHandler::updateInvestigation()
{
	const auto& data = *SharedData::getData();
	const auto majorErrors = (data.errFlags & BMSErrorFlags::maskMajorErrors);

	if (m_errFlags != majorErrors)
	{
		m_errFlags = majorErrors;
		if (m_errFlags) {
			m_investigationTimeout.reset();
		} else {
			changeState(ChargerState::Idle);
		}
	}
	else if (m_investigationTimeout.isReached())
	{
		changeState(ChargerState::Error, m_errFlags);
	}
}

void ChargerHandler::updateError()
{
	m_chargerOnPin.writePin(GPIO_PIN_RESET);
}

void ChargerHandler::changeState(ChargerState state, uint32_t flags)
{
	if (m_state != state)
	{
		if (state == ChargerState::Idle)
		{
			m_chargerOnPin.writePin(GPIO_PIN_SET);
		}
		else if (state == ChargerState::Investigation)
		{
			// 1. 10 сек чекаємо, може роздуплиться
			m_investigationTimeout.reset();
		}
		else if (state == ChargerState::Error)
		{
			m_chargerOnPin.writePin(GPIO_PIN_RESET);
		}

		m_state = state;
		m_errFlags = flags;
	}
}


void ChargerHandler::analyzeBMSData(const BatteryData& data)
{
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
	for (uint16_t voltage: data.cellVoltage) {
		isOvervoltage |= voltage > kOvervoltageSingleValue;
	}

	if (!isOvervoltage) {
		m_overvoltageTimeout.reset();
	} else if (m_overvoltageTimeout.isReached()) {
		changeState(ChargerState::Error);
	}
}

void ChargerHandler::setChargePower(uint16_t watts)
{
	uint16_t value = (watts + 6) / 12; // 1200 Вт - 100
	m_potCS2.setValue(value);
}


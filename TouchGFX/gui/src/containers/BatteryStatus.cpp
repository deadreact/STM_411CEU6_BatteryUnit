#include <gui/containers/BatteryStatus.hpp>
#include <touchgfx/Color.hpp>
#include "stm32f4xx_hal.h"

BatteryStatus::BatteryStatus()
{
	touchgfx::Application::getInstance()->registerTimerWidget(this);
}

BatteryStatus::~BatteryStatus()
{
	touchgfx::Application::getInstance()->unregisterTimerWidget(this);
}

void BatteryStatus::initialize()
{
    BatteryStatusBase::initialize();
}

void BatteryStatus::handleTickEvent()
{
	if (m_state == ChargeState::Charge)
	{
		m_animation.handleTickEvent();
	}
}

void BatteryStatus::setSOC(int val)
{
	if (val != m_soc)
	{
		m_soc = val;
		auto color = touchgfx::Color::getColorFromRGB(255 - val * 2.5, val * 2.5, 5);
		capacityValue.setWidth(10 + val * 0.9f);
		capacityValue.setColor(color);
		animBox.setColor(color);
		Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", val);

		int16_t x = ((100 - m_soc) * 5 + m_soc * 95 + 50)/100;
		int16_t w = ((100 - m_soc) * 10 + m_soc * 4 + 50)/100;
		animBox.setX(x);
		animBox.setWidth(w);

		invalidateContent();
	}
}

void BatteryStatus::setVoltage(int val)
{
	if (val != m_voltage)
	{
		m_voltage = val;
		Unicode::snprintfFloat(voltageValueBuffer, VOLTAGEVALUE_SIZE, "%.2f", val * 0.01f);
		voltageValue.invalidate();
	}
}

void BatteryStatus::setChargeState(ChargeState state)
{
	if (m_state != state)
	{
		m_state = state;
		animBox.setVisible(state == ChargeState::Charge);
		animBox.invalidate();
	}
}


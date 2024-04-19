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
	static int16_t animDuration = 1000;
	static int16_t frameRate = 1000/60;
	if (m_state == ChargeState::Charge)
	{
		int duration = HAL_GetTick() - m_dirChangeTick;
		auto frame = duration / frameRate;
		if (frame != m_lastFrame)
		{
			if (duration >= animDuration) {
				m_dirChangeTick = HAL_GetTick();
				animBox.setAlpha(255);
			} else {
				// ((animDuration - duration) * (-255) + duration * 255)/ animDuration
				auto nAlpha = 510*duration/animDuration - 255;
				animBox.setAlpha(nAlpha < 0 ? -nAlpha : nAlpha);
				m_lastFrame = frame;
			}

			animBox.invalidate();
		}
	}
}

void BatteryStatus::setSOC(int val)
{
	if (val != m_soc)
	{
		m_soc = val;
		auto color = touchgfx::Color::getColorFromRGB(255 - val * 2.5, val * 2.5, 5);
		capacityValue.setWidth(10 + val * 0.9);
		capacityValue.setColor(color);
		Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", val);

		int16_t x = ((100 - m_soc) * 7 + m_soc * 95 + 50)/100;
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
//		animBox.setColor(state == ChargeState::Charge ? 0xff11ea11 : 0xffea1111);
		animBox.setVisible(state == ChargeState::Charge);

//		if (state == ChargeState::Charge) {
//			int16_t x = ((100 - m_soc) * 7 + m_soc * 95 + 50)/100;
//			int16_t w = ((100 - m_soc) * 10 + m_soc * 4 + 50)/100;
//			animBox.setX(x);
//			animBox.setWidth(w);
//		} else if (state == ChargeState::Uncharge) {
//			int16_t x = ((100 - m_soc) * 6 + m_soc * 86 + 50)/100;
//			int16_t w = ((100 - m_soc) * 1 + m_soc * 10 + 50)/100;
//			animBox.setX(x);
//			animBox.setWidth(w);
//		}
		animBox.invalidate();
	}
}


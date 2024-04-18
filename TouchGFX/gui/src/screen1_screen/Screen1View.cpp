#include <gui/screen1_screen/Screen1View.hpp>
#include <shared_data.h>


Screen1View::Screen1View()
	: Screen1ViewBase()
	, textureMapperAnimationEndedCallback(this, &Screen1View::textureMapperAnimationEndedCallbackHandler)
{
	icon_fan.setTextureMapperAnimationEndedAction(textureMapperAnimationEndedCallback);
	icon_inv.setTextureMapperAnimationEndedAction(textureMapperAnimationEndedCallback);
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    if (SharedData::getProcessId() != ProcessId::Idle)
	{
		return;
	}
	const auto& data = SharedData::getData<ProcessId::Idle>();
    updateBatteryData(data.bms);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
    m_bmsData = BatteryData();
//    m_chargeTimeMins = -1;
//    m_isBMSError = false;

}

void Screen1View::handleTickEvent()
{
	Screen1ViewBase::handleTickEvent();
    if (SharedData::getProcessId() == ProcessId::Startup)
    {
//        auto value = SharedData::getData<ProcessId::Startup>().timeLeftToStandby/70;
//        capacityValue.setValue(value);
    }
    if (SharedData::getProcessId() != ProcessId::Idle)
	{
		return;
	}
	const auto& data = SharedData::getData<ProcessId::Idle>();

	bool isMajorError = data.bmsErrFlags & BMSErrorFlags::maskMajorErrors;
	if (m_isBMSError != isMajorError)
	{
		warning.setVisible(isMajorError);
		warning.invalidate();
		m_isBMSError = isMajorError;
	}

	if (data.bms != m_bmsData)
	{
		updateBatteryData(data.bms);
	}
}

void Screen1View::setWatts(int val)
{
	ioValue.setValue((val + 5000) / 10000);
}

void Screen1View::updateBatteryData(const BatteryData& data)
{
	if (data.soc != m_bmsData.soc)
	{
		capacityContainer.setSOC(data.soc);
//		capacityContainerLarge.setValue(data.soc);
	}

	if (data.current != m_bmsData.current)
	{
		setWatts(data.current * data.voltage);
		capacityContainer.setChargeState(data.current > 0 ? ChargeState::Charge : (data.current < 0 ? ChargeState::Uncharge : ChargeState::Idle));
		setIconFanVisible(data.current > 0);
//		setIconInvVisible(true);
	}

	if (data.voltage != m_bmsData.voltage)
	{
		setWatts(data.current * data.voltage);
		capacityContainer.setVoltage(data.voltage);
	}

	int chargeValue = data.calcTimeRemain();
	if (chargeValue != m_chargeTimeMins)
	{
		chargeTimeContainer.setValue(chargeValue);
		m_chargeTimeMins = chargeValue;
	}

	if (m_bmsData.isValid() != data.isValid()) {
		loading.setVisible(!data.isValid());
		content.setVisible(data.isValid());
		invalidate();
	}

	m_bmsData = data;
}

void Screen1View::setIconFanVisible(bool visible)
{
	if (visible != icon_fan.isVisible())
	{
		icon_fan.setVisible(visible);

		if (visible) {
			icon_fan.setupAnimation(touchgfx::AnimationTextureMapper::Z_ROTATION, icon_fan.getZAngle() + PI*2, 80, 0, touchgfx::EasingEquations::linearEaseIn);
			icon_fan.startAnimation();
		} else {
			icon_fan.cancelAnimationTextureMapperAnimation();
		}
	}
}

void Screen1View::setIconInvVisible(bool visible)
{
	if (visible != icon_inv.isVisible())
	{
		icon_inv.setVisible(visible);

		if (visible) {
			auto ease = icon_inv.getScale() > 0 ? touchgfx::EasingEquations::quadEaseOut : touchgfx::EasingEquations::quadEaseIn;
			icon_inv.setupAnimation(touchgfx::AnimationTextureMapper::SCALE, 1.f - icon_inv.getScale(), 40, 0, ease);
			icon_inv.startAnimation();
		} else {
			icon_inv.cancelAnimationTextureMapperAnimation();
		}
	}
}

void Screen1View::textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src)
{
	if (&src == &icon_fan)
	{
		if (icon_fan.isVisible()) {
			icon_fan.setupAnimation(touchgfx::AnimationTextureMapper::Z_ROTATION, icon_fan.getZAngle() + PI*2, 80, 0, touchgfx::EasingEquations::linearEaseIn);
			icon_fan.startAnimation();
		}
	}
	else if (&src == &icon_inv)
	{
		if (icon_inv.isVisible()) {
			auto ease = icon_inv.getScale() > 0 ? touchgfx::EasingEquations::quadEaseOut : touchgfx::EasingEquations::quadEaseIn;
			icon_inv.setupAnimation(touchgfx::AnimationTextureMapper::SCALE, 1.f - icon_inv.getScale(), 40, 0, ease);
			icon_inv.startAnimation();
		}
	}
}


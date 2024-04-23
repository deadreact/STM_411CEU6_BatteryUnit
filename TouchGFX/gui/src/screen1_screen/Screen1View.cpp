#include <gui/screen1_screen/Screen1View.hpp>
#include <shared_data.h>


Screen1View::Screen1View()
	: Screen1ViewBase()
	, m_invState{InverterState::Off}
	, textureMapperAnimationEndedCallback(this, &Screen1View::textureMapperAnimationEndedCallbackHandler)
{
	icon_fan.setTextureMapperAnimationEndedAction(textureMapperAnimationEndedCallback);
//	icon_inv.setTextureMapperAnimationEndedAction(textureMapperAnimationEndedCallback);
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    if (SharedData::getProcessId() != ProcessId::Idle)
	{
		return;
	}
	const auto& data = SharedData::getData<ProcessId::Idle>();
	auto smoothedCurr = data.smoothedCurrent.get();
    updateBatteryData(data.bms, data.isBMSDataValid() ? smoothedCurr: 0);
    showLoading(!data.isBMSDataValid());
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

	bool isMajorError = data.errFlags & BMSErrorFlags::maskMajorErrors;
	if (m_isBMSError != isMajorError)
	{
		icon_warn.setVisible(isMajorError);
		icon_warn.invalidate();
		m_isBMSError = isMajorError;
	}

	bool isChargError = data.errFlags & 0x01000000;
	if (m_isChargError != isChargError)
	{
		icon_chargErr.setVisible(isChargError);
		icon_chargErr.invalidate();
		m_isChargError = isChargError;
	}

	auto smoothedCurr = (m_isBMSError || !data.isBMSDataValid()) ? 0 : data.smoothedCurrent.get();
	if (data.bms != m_bmsData || smoothedCurr != m_bmsData.current)
	{
		updateBatteryData(data.bms, smoothedCurr);
	}

	if (m_invState != data.invState) {
		m_invState = data.invState;
		updateInvState();
	}

	if (m_usbState != data.usbState) {
		m_usbState = data.usbState;
		icon_usb.setVisible(m_usbState);
		icon_usb.invalidate();
	}

	if (data.chargerPlugged != icon_chargPlug.isVisible())
	{
		icon_chargPlug.setVisible(data.chargerPlugged);
		icon_chargPlug.invalidate();
	}

	if (loading.isVisible() && m_loadingAnimTimeout.isReached())
	{
		loading.setZAngle(loading.getZAngle() + PI/6);
		loading.invalidate();
		m_loadingAnimTimeout.reset();
	}

	if (m_invState == InverterState::Intermediate)
	{
		m_invAnimation.handleTickEvent();
	}

	if (m_isBMSError)
	{
		m_warnAnimation.handleTickEvent();
	}
}

void Screen1View::setWatts(int val)
{
	ioValue.setValue((val + 5000) / 10000);
}

void Screen1View::updateBatteryData(const BatteryData& data, int16_t smoothedCurr)
{


	if (data.soc != m_bmsData.soc)
	{
		capacityContainer.setSOC(data.soc);
//		capacityContainerLarge.setValue(data.soc);
	}

	if (smoothedCurr != m_bmsData.current)
	{
		setWatts(smoothedCurr * data.voltage);
		capacityContainer.setChargeState(smoothedCurr > 0 ? ChargeState::Charge : (smoothedCurr < 0 ? ChargeState::Uncharge : ChargeState::Idle));
		setIconFanVisible(smoothedCurr > 0);
//		updateInvState(true);
	}

	if (data.voltage != m_bmsData.voltage)
	{
		setWatts(smoothedCurr * data.voltage);
		capacityContainer.setVoltage(data.voltage);
	}

	int chargeValue = data.calcTimeRemain(smoothedCurr);
	if (chargeValue != m_chargeTimeSec)
	{
		chargeTimeContainer.setValue(chargeValue);
		m_chargeTimeSec = chargeValue;
	}

	// TODO: костиль!!
	const auto& sharedData = SharedData::getData<ProcessId::Idle>();
	if (sharedData.isBMSDataValid(&m_bmsData) != sharedData.isBMSDataValid(&data)) {
		showLoading(!sharedData.isBMSDataValid(&data));
	}

	m_bmsData = data;
	m_bmsData.current = smoothedCurr;
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

void Screen1View::updateInvState()
{
	icon_inv.setVisible(m_invState != InverterState::Off);

	if (m_invState == InverterState::On)
	{
		icon_inv.setAlpha(255);
	}
	icon_inv.invalidate();
}

void Screen1View::showLoading(bool show)
{
	loading.setVisible(show);
	loading_bg.setVisible(show);
	invalidate();
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
//	else if (&src == &icon_inv)
//	{
//		if (m_invState == InverterState::Intermediate) {
//			auto ease = icon_inv.getScale() > 0 ? touchgfx::EasingEquations::quadEaseOut : touchgfx::EasingEquations::quadEaseIn;
//			icon_inv.setupAnimation(touchgfx::AnimationTextureMapper::SCALE, 1.f - icon_inv.getScale(), 40, 0, ease);
//			icon_inv.startAnimation();
//		}
//	}
}


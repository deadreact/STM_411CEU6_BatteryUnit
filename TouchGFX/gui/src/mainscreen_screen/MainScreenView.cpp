#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <shared_data.h>
#include <cmsis_os.h>

MainScreenView::MainScreenView()
    : m_powerModeState(PowerModeState::Normal)
    , m_invState{InverterState::Off}
	, m_msgMoveAnimationEndedCallback(this, &MainScreenView::msgMoveAnimationEndedHandler)
//    , textureMapperAnimationEndedCallback(this, &MainScreenView::textureMapperAnimationEndedCallbackHandler)
{
//    icon_fan.setTextureMapperAnimationEndedAction(textureMapperAnimationEndedCallback);
	container_popup.setMoveAnimationEndedAction(m_msgMoveAnimationEndedCallback);
}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();

    while (!SharedData::getData()) {
    	osThreadYield();
    }

    const auto& data = *SharedData::getData();
    const auto soc = m_bmsData.soc;
    m_bmsData = BatteryData();
    m_bmsData.soc = soc;
    updateBatteryData(data.bms);
    showLoading(!data.bms.isValid() || m_powerModeState == PowerModeState::WakedUp);
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

void MainScreenView::handleTickEvent()
{
    MainScreenViewBase::handleTickEvent();

    if (!SharedData::getData()) {
    	return;
    }

    const auto& data = *SharedData::getData();

    if (m_powerModeState != data.getPowerModeState())
    {
        m_powerModeState = data.getPowerModeState();
        setupScreen();
    }

    bool isMajorError = data.errFlags & BMSErrorFlags::maskMajorErrors;
    if (m_isBMSError != isMajorError)
    {
    	if (isMajorError) {
    		showWarning(data.errMsg.c_str());
    	} else {
    		hideWarning();
    	}

//        icon_warn.setVisible(isMajorError);
//        icon_warn.invalidate();
        m_isBMSError = isMajorError;
    }

    bool isChargError = data.errFlags & 0x01000000;
    if (m_isChargError != isChargError)
    {
//        icon_chargErr.setVisible(isChargError);
//        icon_chargErr.invalidate();
        m_isChargError = isChargError;
    }

    if (data.bms != m_bmsData)
    {
        updateBatteryData(data.bms);
        showLoading(!data.bms.isValid());
    }

    if (loading.isVisible() && m_loadingAnimTimeout.isReached())
    {
        static const float kAngle = PI/6;
        loading.setZAngle(loading.getZAngle() + kAngle);
        loading.invalidate();
        m_loadingAnimTimeout.reset();
    }

    if (m_invState != data.invState) {
        m_invState = data.invState;
        updateInvState();
    }

	if (m_usbState != data.usbState) {
		m_usbState = data.usbState;
		icon_usb.setAlpha(m_usbState ? 255 : 51);
		icon_usb.invalidate();
	}

	if (m_fan != data.fan) {
		m_fan = data.fan;
		icon_fan.setAlpha(m_fan > 0 ? 205 + m_fan/2 : 51);
		icon_fan.invalidate();
	}

    if (data.chargerPlugged != icon_charge.isVisible())
    {
        icon_charge.setVisible(data.chargerPlugged);
        icon_charge.invalidate();
    }

    if (m_invState == InverterState::Intermediate)
    {
        m_invAnimation.handleTickEvent();
    }

    if (m_isBMSError)
    {
//        m_warnAnimation.handleTickEvent();
    }
}

void MainScreenView::setPower(int val)
{

	if (val != m_power)
	{
		int power = val = (val + 5000) / 10000;
		touchgfx::Unicode::snprintf(power_valueBuffer, POWER_VALUE_SIZE, "%d", power < 0 ? -power: power);
		if ((val < 0 && m_power >= 0) || (val > 0 && m_power <= 0) || val == 0)
		{
			label_charging.setVisible(val > 0);
			label_discharging.setVisible(val < 0);
			label_charging.invalidate();
			label_discharging.invalidate();
		}

		m_power = val;
		power_value.invalidate();
	}
}

void MainScreenView::showWarning(const char* text)
{
	memset(popup_textBuffer,0, POPUP_TEXT_SIZE * 2);

	auto* buffer = &popup_textBuffer[0];
	for (; *text != '\0'; buffer++, text++) {
		*buffer = *text;
	}

	m_showWarning = true;
	container_popup.setVisible(true);

	if (container_popup.isMoveAnimationRunning())
	{
		container_popup.cancelMoveAnimation();
	}

	container_popup.startMoveAnimation(0, 200, 20, &EasingEquations::backEaseIn, &EasingEquations::expoEaseIn);
}

void MainScreenView::hideWarning()
{
	m_showWarning = false;

	if (container_popup.isMoveAnimationRunning())
	{
		container_popup.cancelMoveAnimation();
	}
	container_popup.startMoveAnimation(0, 240, 20, &EasingEquations::backEaseIn, &EasingEquations::expoEaseIn);
}

void MainScreenView::updateBatteryData(const BatteryData& data)
{
    const int16_t current = m_isBMSError ? 0 : data.current;
    if (data.soc != m_bmsData.soc)
    {
        if (data.isValid()) {
        	batteryMainProgressBar.setProgress(data.soc);
        }
    }

    if (current != m_bmsData.current || data.voltage != m_bmsData.voltage)
    {
    	setPower(current * data.voltage);
    }

    int chargeValue = data.calcTimeRemain(current);
    if (chargeValue != m_chargeTimeSec)
    {
    	const int absVal = chargeValue < 0 ? -chargeValue : chargeValue;
		const int hours = absVal / SEC_IN_HOUR;
		const int mins = (absVal % SEC_IN_HOUR) / SEC_IN_MIN;
		touchgfx::Unicode::snprintf(hours_value_1Buffer, HOURS_VALUE_1_SIZE, "%d", hours/10);
		touchgfx::Unicode::snprintf(hours_value_2Buffer, HOURS_VALUE_2_SIZE, "%d", hours%10);
		touchgfx::Unicode::snprintf(minutes_value_1Buffer, MINUTES_VALUE_1_SIZE, "%d", mins/10);
		touchgfx::Unicode::snprintf(minutes_value_2Buffer, MINUTES_VALUE_2_SIZE, "%d", mins%10);
		m_chargeTimeSec = chargeValue;
		container_timer.invalidate();
    }

    m_bmsData = data;
}

void MainScreenView::setIconFanVisible(bool visible)
{
//    if (visible != icon_fan.isVisible())
//    {
//        icon_fan.setVisible(visible);
//
//        if (visible) {
//            icon_fan.setupAnimation(touchgfx::AnimationTextureMapper::Z_ROTATION, icon_fan.getZAngle() + PI*2, 80, 0, touchgfx::EasingEquations::linearEaseIn);
//            icon_fan.startAnimation();
//        } else {
//            icon_fan.cancelAnimationTextureMapperAnimation();
//        }
//    }
}

void MainScreenView::updateInvState()
{
    icon_inv.setVisible(m_invState != InverterState::Off);

    if (m_invState == InverterState::On)
    {
        icon_inv.setAlpha(0xff);
    }
    icon_inv.invalidate();
}

void MainScreenView::showLoading(bool show)
{
    if (loading.isVisible() != show)
    {
        loading.setVisible(show);
        loading_bg.setVisible(show);
        invalidate();
    }
}

void MainScreenView::msgMoveAnimationEndedHandler(const touchgfx::MoveAnimator<touchgfx::Container>& src)
{
    if (&src == &container_popup)
    {
    	container_popup.setVisible(m_showWarning);
    }
}

//void MainScreenView::textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src)
//{
//    if (&src == &icon_fan)
//    {
//        if (icon_fan.isVisible()) {
//            icon_fan.setupAnimation(touchgfx::AnimationTextureMapper::Z_ROTATION, icon_fan.getZAngle() + PI*2, 80, 0, touchgfx::EasingEquations::linearEaseIn);
//            icon_fan.startAnimation();
//        }
//    }
//}


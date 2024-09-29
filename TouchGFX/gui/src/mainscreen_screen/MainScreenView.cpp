#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <shared_data.h>

MainScreenView::MainScreenView()
    : MainScreenViewBase()
    , m_powerModeState(PowerModeState::Normal)
    , m_invState{InverterState::Off}
    , textureMapperAnimationEndedCallback(this, &MainScreenView::textureMapperAnimationEndedCallbackHandler)
{
    icon_fan.setTextureMapperAnimationEndedAction(textureMapperAnimationEndedCallback);

//    m_settingsPopUp = new Settings;
//    m_settingsPopUp->setXY(40, 40);
//    add(*m_settingsPopUp);
}

MainScreenView::~MainScreenView()
{
//	delete m_settingsPopUp;
}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();

    const auto& data = SharedData::getData();
    const auto soc = m_bmsData.soc;
    m_bmsData = BatteryData();
    m_bmsData.soc = soc;
    updateBatteryData(data.bms);
    showLoading(!data.bms.isValid() || m_powerModeState == PowerModeState::WakedUp);

//    m_settingsPopUp->initialize();
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}

void MainScreenView::handleTickEvent()
{
    MainScreenViewBase::handleTickEvent();

    const auto& data = SharedData::getData();

    if (m_powerModeState != data.getPowerModeState())
    {
        m_powerModeState = data.getPowerModeState();
        setupScreen();
    }

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

    if (data.bms != m_bmsData)
    {
        updateBatteryData(data.bms);
        showLoading(!data.bms.isValid());
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
        static const float kAngle = PI/6;
        loading.setZAngle(loading.getZAngle() + kAngle);
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

void MainScreenView::setWatts(int val)
{
    ioValue.setValue((val + 5000) / 10000);
}

Settings* MainScreenView::takeSettings()
{
	if (m_settingsPopUp)
	{
		remove(*m_settingsPopUp);
		auto tmp = m_settingsPopUp;
		m_settingsPopUp = nullptr;
		invalidate();
		return tmp;
	}
	return nullptr;
}

void MainScreenView::setSettings(Settings* settings)
{
	if (settings)
	{
		m_settingsPopUp = settings;
		add(*settings);
		invalidate();
	}
}

void MainScreenView::updateBatteryData(const BatteryData& data)
{
    const int16_t smoothedCurr = m_isBMSError ? 0 : data.current;
    if (data.soc != m_bmsData.soc)
    {
        if (data.isValid()) {
            capacityContainer.setSOC(data.soc);
        }
    }

    if (smoothedCurr != m_bmsData.current)
    {
        setWatts(smoothedCurr * data.voltage);
        capacityContainer.setChargeState(smoothedCurr > 0 ? ChargeState::Charge : (smoothedCurr < 0 ? ChargeState::Uncharge : ChargeState::Idle));
        setIconFanVisible(smoothedCurr > 0);
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

    m_bmsData = data;
    m_bmsData.current = smoothedCurr;
}

void MainScreenView::setIconFanVisible(bool visible)
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


void MainScreenView::textureMapperAnimationEndedCallbackHandler(const touchgfx::AnimationTextureMapper& src)
{
    if (&src == &icon_fan)
    {
        if (icon_fan.isVisible()) {
            icon_fan.setupAnimation(touchgfx::AnimationTextureMapper::Z_ROTATION, icon_fan.getZAngle() + PI*2, 80, 0, touchgfx::EasingEquations::linearEaseIn);
            icon_fan.startAnimation();
        }
    }
}


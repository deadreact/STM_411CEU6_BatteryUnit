#ifndef DEBUGSCREENVIEW_HPP
#define DEBUGSCREENVIEW_HPP

#include <gui_generated/debugscreen_screen/DebugScreenViewBase.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>
#include <gui/containers/Settings.hpp>
#include <gui/common/CommonScreenView.h>
#include <gui/common/BlinkingAnimation.h>

#include <bms_data.h>
#include <gui/containers/BatteryCell.hpp>

class DebugScreenView : public CommonScreenView<DebugScreenViewBase>
{
public:
    DebugScreenView();
    virtual ~DebugScreenView();
    virtual void setupScreen();
	virtual void tearDownScreen();
	virtual void handleTickEvent() override;

private:
	void colorizeCells();
	void updateInvState();
protected:
	BatteryData m_bmsData;
	utils::stack_string errMsg;
	int16_t temperatureInv{0};
	int16_t temperatureCharg{0};
	InverterState m_invState{InverterState::Off};
	bool m_usbState{false};
	uint8_t m_fan{0};
//	uint8_t fan{0};

	int m_chargeTimeSec{-1};
private:
	BlinkingAnimation<touchgfx::Image> m_invAnimation{icon_inv, 800};
};

#endif // DEBUGSCREENVIEW_HPP

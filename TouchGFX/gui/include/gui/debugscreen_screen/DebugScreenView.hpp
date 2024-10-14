#ifndef DEBUGSCREENVIEW_HPP
#define DEBUGSCREENVIEW_HPP

#include <gui_generated/debugscreen_screen/DebugScreenViewBase.hpp>
#include <gui/debugscreen_screen/DebugScreenPresenter.hpp>
#include <gui/containers/Settings.hpp>
#include <gui/common/CommonScreenView.h>

#include <bms_data.h>
#include <gui/containers/BatteryCellElement.hpp>

class DebugScreenView : public CommonScreenView<DebugScreenViewBase>
{
public:
    DebugScreenView();
    virtual ~DebugScreenView();
    virtual void setupScreen();
	virtual void tearDownScreen();
	virtual void handleTickEvent() override;

	void colorizeCells();
protected:
	BatteryData m_bmsData;
	utils::stack_string errMsg;
	uint8_t temperatureInv{0};
	uint8_t temperatureCharg{0};
	uint8_t fan{0};
private:
	BatteryCellElement cell[BatteryData::kMaxCellCount];
};

#endif // DEBUGSCREENVIEW_HPP

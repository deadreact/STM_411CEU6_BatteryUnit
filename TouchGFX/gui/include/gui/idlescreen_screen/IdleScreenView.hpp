#ifndef IDLESCREENVIEW_HPP
#define IDLESCREENVIEW_HPP

#include <gui_generated/idlescreen_screen/IdleScreenViewBase.hpp>
#include <gui/idlescreen_screen/IdleScreenPresenter.hpp>
#include <gui/containers/BatteryCellElement.hpp>
#include <bms_data.h>

class IdleScreenView : public IdleScreenViewBase
{
public:
    IdleScreenView();
    virtual ~IdleScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;
protected:
    BatteryData m_bmsData;
//    int capacity {0};
//    int voltage {0};
//    float current {0.f};
    char errMsg[40];

//    uint8_t cellCount{0};
//    uint16_t cellVoltage[12];
private:
    BatteryCellElement cell[BatteryData::kMaxCellCount];
};

#endif // IDLESCREENVIEW_HPP

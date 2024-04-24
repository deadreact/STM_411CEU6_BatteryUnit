#ifndef IDLESCREENVIEW_HPP
#define IDLESCREENVIEW_HPP

#include <bms/bms_data.h>
#include <gui_generated/idlescreen_screen/IdleScreenViewBase.hpp>
#include <gui/idlescreen_screen/IdleScreenPresenter.hpp>
#include <gui/containers/BatteryCellElement.hpp>

class IdleScreenView : public IdleScreenViewBase
{
public:
    IdleScreenView();
    virtual ~IdleScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent() override;

    void colorizeCells();
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

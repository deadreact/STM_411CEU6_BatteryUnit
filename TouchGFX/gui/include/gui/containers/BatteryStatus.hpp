#ifndef BATTERYSTATUS_HPP
#define BATTERYSTATUS_HPP

#include <gui_generated/containers/BatteryStatusBase.hpp>
#include <gui/common/common.h>

class BatteryStatus : public BatteryStatusBase
{
public:
    BatteryStatus();
    virtual ~BatteryStatus();

    virtual void initialize();
    virtual void handleTickEvent() override;
    void setSOC(int val);
    void setVoltage(int val);
    void setChargeState(ChargeState state);
protected:
    int m_soc{0};
    int m_voltage{0};
    ChargeState m_state{InvalidChargeState};

    // animation
    uint32_t m_dirChangeTick{0};
    uint8_t m_lastFrame{0};
};

#endif // BATTERYSTATUS_HPP

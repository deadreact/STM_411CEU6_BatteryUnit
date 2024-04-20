#ifndef BATTERYSTATUS_HPP
#define BATTERYSTATUS_HPP

#include <gui_generated/containers/BatteryStatusBase.hpp>
#include <gui/common/common.h>
#include <gui/common/BlinkingAnimation.h>


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

    BlinkingAnimation<touchgfx::Box> m_animation{animBox, 1000};
};

#endif // BATTERYSTATUS_HPP

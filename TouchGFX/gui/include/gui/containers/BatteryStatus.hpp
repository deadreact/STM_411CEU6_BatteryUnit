#ifndef BATTERYSTATUS_HPP
#define BATTERYSTATUS_HPP

#include <gui_generated/containers/BatteryStatusBase.hpp>

class BatteryStatus : public BatteryStatusBase
{
public:
    BatteryStatus();
    virtual ~BatteryStatus() {}

    virtual void initialize();
    void setValue(int val);
protected:
    int m_value{0};
};

#endif // BATTERYSTATUS_HPP

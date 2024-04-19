#ifndef BATTERYSTATUSLARGE_HPP
#define BATTERYSTATUSLARGE_HPP

#include <gui_generated/containers/BatteryStatusLargeBase.hpp>

class BatteryStatusLarge : public BatteryStatusLargeBase
{
public:
    BatteryStatusLarge();
    virtual ~BatteryStatusLarge() {}

    virtual void initialize();
    void setValue(int val);
protected:
    int m_value{0};
};

#endif // BATTERYSTATUSLARGE_HPP

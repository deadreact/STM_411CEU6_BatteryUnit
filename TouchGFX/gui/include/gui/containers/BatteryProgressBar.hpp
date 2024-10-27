#ifndef BATTERYPROGRESSBAR_HPP
#define BATTERYPROGRESSBAR_HPP

#include <gui_generated/containers/BatteryProgressBarBase.hpp>

class BatteryProgressBar : public BatteryProgressBarBase
{
public:
    BatteryProgressBar();
    virtual ~BatteryProgressBar() {}

    virtual void initialize();

    void setProgress(uint8_t progress);
protected:
    uint8_t m_progress{50};
};

#endif // BATTERYPROGRESSBAR_HPP

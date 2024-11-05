#ifndef BATTERYMAINPROGRESSBAR_HPP
#define BATTERYMAINPROGRESSBAR_HPP

#include <gui_generated/containers/BatteryMainProgressBarBase.hpp>

class BatteryMainProgressBar : public BatteryMainProgressBarBase
{
public:
    BatteryMainProgressBar();
    virtual ~BatteryMainProgressBar() {}

    virtual void initialize();
    void setProgress(uint8_t progress);
protected:
    uint8_t m_progress{50};
};

#endif // BATTERYMAINPROGRESSBAR_HPP

#ifndef BATTERYMAINELEMENT_HPP
#define BATTERYMAINELEMENT_HPP

#include <gui_generated/containers/BatteryMainElementBase.hpp>

class BatteryMainElement : public BatteryMainElementBase
{
public:
    BatteryMainElement();
    virtual ~BatteryMainElement() {}

    virtual void initialize();
protected:
};

#endif // BATTERYMAINELEMENT_HPP

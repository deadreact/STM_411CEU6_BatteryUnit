#ifndef BATTERYCELLELEMENT_HPP
#define BATTERYCELLELEMENT_HPP

#include <gui_generated/containers/BatteryCellElementBase.hpp>

class BatteryCellElement : public BatteryCellElementBase
{
public:
    BatteryCellElement();
    virtual ~BatteryCellElement() {}

    void setIndex(int idx);
    void setVoltage(float voltage);

    virtual void initialize();
protected:
};

#endif // BATTERYCELLELEMENT_HPP

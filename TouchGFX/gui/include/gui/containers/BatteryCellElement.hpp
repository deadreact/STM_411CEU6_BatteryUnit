#ifndef BATTERYCELLELEMENT_HPP
#define BATTERYCELLELEMENT_HPP

#include <gui_generated/containers/BatteryCellElementBase.hpp>

enum class CellMarker: uint32_t
{
	Max = 0xff11ed11,
	Min = 0xffed1111,
	Average = 0xffe8f6fb
};

class BatteryCellElement : public BatteryCellElementBase
{
public:
    BatteryCellElement();
    virtual ~BatteryCellElement() {}

    void setIndex(int idx);
    void setVoltage(float voltage);
    void setMarker(CellMarker marker);

    virtual void initialize();
protected:
    CellMarker m_marker{CellMarker::Average};
};

#endif // BATTERYCELLELEMENT_HPP

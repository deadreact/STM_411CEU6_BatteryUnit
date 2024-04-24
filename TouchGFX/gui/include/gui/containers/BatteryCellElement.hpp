#ifndef BATTERYCELLELEMENT_HPP
#define BATTERYCELLELEMENT_HPP

#include <gui_generated/containers/BatteryCellElementBase.hpp>

enum class CellMarker
{
	Max, Min, Average
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

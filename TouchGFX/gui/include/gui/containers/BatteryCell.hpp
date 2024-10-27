#ifndef BATTERYCELL_HPP
#define BATTERYCELL_HPP

#include <gui_generated/containers/BatteryCellBase.hpp>

enum class CellMarker: uint32_t
{
	Max = 0xFF00F54D,
	Min = 0xFFFF2614,
	Average = 0xFFFFF5D9
};

class BatteryCell : public BatteryCellBase
{
public:
    BatteryCell();
    virtual ~BatteryCell() {}

    void setIndex(int idx);
    void setVoltage(float voltage);
    void setMarker(CellMarker marker);

    virtual void initialize();
protected:
    CellMarker m_marker{CellMarker::Average};
};

#endif // BATTERYCELL_HPP

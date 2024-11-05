#include <gui/containers/BatteryCell.hpp>
#include <images/BitmapDatabase.hpp>

BatteryCell::BatteryCell()
{

}

void BatteryCell::initialize()
{
    BatteryCellBase::initialize();
}


void BatteryCell::setIndex(int idx)
{
	Unicode::snprintf(indexBuffer, INDEX_SIZE, "%d", idx);;
	index.invalidate();
}

void BatteryCell::setVoltage(float voltage)
{
	progress.setValue(voltage * 100);
	progress.setBitmap(voltage < 2.8f ? BITMAP_BATTERY_FILL_RED_ID : BITMAP_BATTERY_FILL_WHITE_ID);

	Unicode::snprintfFloat(valueBuffer, VALUE_SIZE, "%.4f", voltage);
	value.invalidate();
}


void BatteryCell::setMarker(CellMarker marker)
{
	if (m_marker != marker)
	{
		m_marker = marker;

		value.setColor(static_cast<uint32_t>(marker));

		value.invalidate();
	}
}

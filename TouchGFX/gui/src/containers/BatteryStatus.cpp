#include <gui/containers/BatteryStatus.hpp>
#include <touchgfx/Color.hpp>

BatteryStatus::BatteryStatus()
{

}

void BatteryStatus::initialize()
{
    BatteryStatusBase::initialize();
}

void BatteryStatus::setValue(int val)
{
	if (val != m_value)
	{
		m_value = val;
		capacityValue.setWidth(val);
		capacityValue.setColor(touchgfx::Color::getColorFromRGB(255 - val * 2.5, val * 2.5, 5));
		Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", val);
		invalidateContent();
	}
}

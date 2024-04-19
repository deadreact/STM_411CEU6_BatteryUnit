#include <gui/containers/BatteryStatusLarge.hpp>
#include <touchgfx/Color.hpp>

BatteryStatusLarge::BatteryStatusLarge()
{

}

void BatteryStatusLarge::initialize()
{
    BatteryStatusLargeBase::initialize();
}

void BatteryStatusLarge::setValue(int val)
{
	if (val != m_value)
	{
		m_value = val;
		capacityValue.setWidth(val * 2.36f);
		capacityValue.setColor(touchgfx::Color::getColorFromRGB(255 - val * 2.5f, val * 2.5f, 5));
		Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", val);
		invalidateContent();
	}
}

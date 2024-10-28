#include <gui/containers/TemperatureContainer.hpp>

TemperatureContainer::TemperatureContainer()
{

}

void TemperatureContainer::initialize()
{
    TemperatureContainerBase::initialize();
}

void TemperatureContainer::setValue(int16_t val)
{
	if (m_value != val)
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", val);
		value.setColor((val > m_min && val < m_max) ? 0xFFFFF5D9 : 0xFFFF2614);
		value.invalidate();
		m_value = val;
	}
}

void TemperatureContainer::setTitle(TypedTextId textId)
{
	if (title.getTypedText().getId() != textId)
	{
		title.setTypedText(touchgfx::TypedText(textId));
		title.invalidate();
	}
}

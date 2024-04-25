#include <gui/containers/BatteryCellElement.hpp>

BatteryCellElement::BatteryCellElement()
{

}

void BatteryCellElement::setIndex(int idx)
{
	Unicode::snprintf(indexBuffer, INDEX_SIZE, "%d", idx);;
	index.invalidate();
}

void BatteryCellElement::setVoltage(float voltage)
{
	Unicode::snprintfFloat(valueBuffer, VALUE_SIZE, "%.4f", voltage);
	value.invalidate();
}

void BatteryCellElement::initialize()
{
    BatteryCellElementBase::initialize();
}


void BatteryCellElement::setMarker(CellMarker marker)
{
	if (m_marker != marker)
	{
		m_marker = marker;

		value.setColor(static_cast<uint32_t>(marker));

		value.invalidate();
	}
}

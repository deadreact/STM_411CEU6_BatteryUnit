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

		switch (marker)
		{
		case CellMarker::Average:
			value.setColor(0xFFE8F6FB);
			break;
		case CellMarker::Min:
			value.setColor(0xffed1111);
			break;
		case CellMarker::Max:
			value.setColor(0xff11ed11);
			break;
		}

		value.invalidate();
	}
}

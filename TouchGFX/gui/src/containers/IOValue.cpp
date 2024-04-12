#include <gui/containers/IOValue.hpp>
#include <texts/TextKeysAndLanguages.hpp>

constexpr static const uint8_t kZeroAlpha = 50;

IOValue::IOValue()
{

}

void IOValue::initialize()
{
    IOValueBase::initialize();
}

void IOValue::setValue(int val)
{
	if (m_value != val)
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%04d", val);

		if (val == 0)
		{
			ioLabel.setAlpha(kZeroAlpha);
			value.setAlpha(kZeroAlpha);
			Watts.setAlpha(kZeroAlpha);
			invalidate();
		}
		else if (m_value == 0)
		{
			ioLabel.setAlpha(255);
			value.setAlpha(255);
			Watts.setAlpha(255);
			invalidate();
		}
		else
		{
			value.invalidate();
		}
		m_value = val;
	}
}
void IOValue::setIO(IO io)
{
	TypedTextId textId = io == In ? T_INPUT : T_OUTPUT;
	if (ioLabel.getTypedText().getId() != textId)
	{
		ioLabel.setTypedText(touchgfx::TypedText(textId));
		ioLabel.resizeToCurrentTextWithAlignment();
		ioLabel.setX(io == In ? 108 : 92);
		value.setX(io == In ? 10 : 0);
		ioLabel.invalidate();
		value.invalidate();
	}
}

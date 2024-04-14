#include <gui/containers/IOValue.hpp>
#include <texts/TextKeysAndLanguages.hpp>

constexpr static const uint8_t kZeroAlpha = 44;

IOValue::IOValue()
{

}

void IOValue::initialize()
{
    IOValueBase::initialize();
}

void IOValue::setValue(float val)
{
	if (m_value != val)
	{
		Unicode::snprintfFloat(valueBuffer, VALUE_SIZE, "%.1f", val);

		setState(val < 0 ? State::Uncharge : (val > 0 ? State::Charge : State::Idle));
		value.invalidate();

		m_value = val;
	}
}
void IOValue::setState(State state)
{
	if (m_state != state)
	{
		ioLabel.setVisible(state != State::Idle);
		value.setAlpha(state != State::Idle ? 255 : kZeroAlpha);
		Watts.setAlpha(state != State::Idle ? 255 : kZeroAlpha);
//#8EEDA6 #ED8E8E

		if (state != State::Idle)
		{
			TypedTextId textId = state == State::Charge ? T_INPUT : T_OUTPUT;
			if (ioLabel.getTypedText().getId() != textId)
			{
				ioLabel.setTypedText(touchgfx::TypedText(textId));
				ioLabel.resizeToCurrentTextWithAlignment();
				ioLabel.setX(state == State::Charge ? 175 : 163);
				Watts.setX(state == State::Charge ? 175 : 163);
				value.setX(state == State::Charge ? 10 : 0);
			}
		}
		m_state = state;

		invalidate();
	}
}

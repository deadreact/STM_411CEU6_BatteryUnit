#include <gui/containers/IOValue.hpp>
#include <texts/TextKeysAndLanguages.hpp>

IOValue::IOValue()
{
	setState(ChargeState::Idle);
}

void IOValue::initialize()
{
    IOValueBase::initialize();
}

void IOValue::setValue(int val)
{
	if (m_value != val)
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", val < 0 ? -val : val);

		setState(val < 0 ? ChargeState::Uncharge : (val > 0 ? ChargeState::Charge : ChargeState::Idle));
		value.invalidate();

		m_value = val;
	}
}
void IOValue::setState(ChargeState state)
{
	if (m_state != state)
	{
		ioLabel.setVisible(state != ChargeState::Idle);
		value.setVisible(state != ChargeState::Idle);
		Watts.setVisible(state != ChargeState::Idle);

		if (state != ChargeState::Idle)
		{
			TypedTextId textId = state == ChargeState::Charge ? T_INPUT : T_OUTPUT;
			uint32_t color = state == ChargeState::Charge ? 0xff11ed11 : 0xffed1111;
			if (ioLabel.getTypedText().getId() != textId)
			{
				ioLabel.setTypedText(touchgfx::TypedText(textId));
				ioLabel.setColor(color);
			}
		}
		m_state = state;

		invalidate();
	}
}

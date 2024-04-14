#include <gui/containers/ChargeTime.hpp>
#include <texts/TextKeysAndLanguages.hpp>


constexpr static const uint8_t kZeroAlpha = 44;
constexpr static const uint32_t kBlue = 0xFF8FB5EF;
constexpr static const uint32_t kYellow = 0xFFFC9600;
constexpr static const uint32_t kRed = 0xFFFC0005;

constexpr static const uint32_t kLabelBlue = 0xFF687FCC;

ChargeTime::ChargeTime()
{

}

void ChargeTime::initialize()
{
    ChargeTimeBase::initialize();
}

void ChargeTime::setValue(int val)
{
	val = std::min(999, std::max(-999, val));
	if (val != m_value)
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", (val < 0 ? -val : val));
		m_value = val;
		//#CC6868

		if (val >= 0 || val < -kYellowTreshold)
		{
			value.setColor(kBlue);
			chargeTimeLabel.setColor(kLabelBlue);
			minsLabel.setColor(kLabelBlue);
		}
		else if (val < -kRedTreshold)
		{
			value.setColor(kYellow);
			chargeTimeLabel.setColor(kYellow);
			minsLabel.setColor(kYellow);

		}
		else if (val < 0)
		{
			value.setColor(kRed);
			chargeTimeLabel.setColor(kRed);
			minsLabel.setColor(kRed);
		}

		chargeTimeLabel.setTypedText(touchgfx::TypedText(val > 0 ? T_RECHARGING_TIME : T_REMAINING_TIME));
		chargeTimeLabel.resizeToCurrentTextWithAlignment();

		warningYellow.setVisible(val >= -kYellowTreshold && val < -kRedTreshold);
		warningRed.setVisible(val >= -kRedTreshold && val < 0);

		uint8_t alpha = (val == 0 || val == 999 || val == -999) ? kZeroAlpha : 255;
		value.setAlpha(alpha);
		chargeTimeLabel.setAlpha(alpha);
		minsLabel.setAlpha(alpha);

		invalidate();
	}
}

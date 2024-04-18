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
//	val = std::min(999, std::max(-999, val));
	if (val != m_value)
	{
		const int absVal = val < 0 ? -val : val;
		int hours = absVal / 60;
		const int mins = absVal % 60;

		Format format;
		if (absVal > 9 * 60 + 30) {
			if (hours > 24) {
				format = Format::Maximum;
			} else {
				hours = (absVal + 30)/ 60;
				format = Format::Hours;
			}
		} else if (absVal > 59) {
			format = Format::Full;
		} else if (absVal > 1) {
			format = Format::Minutes;
		} else if (absVal > 0) {
			format = Format::Minimum;
		} else {
			format = Format::Hidden;
		}

		setFormat(format);
		setTime(hours, mins);

//		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", (val < 0 ? -val : val));
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

//		uint8_t alpha = (val == 0 || val == 999 || val == -999) ? kZeroAlpha : 255;
//		value.setAlpha(alpha);
//		chargeTimeLabel.setAlpha(alpha);
//		minsLabel.setAlpha(alpha);

		invalidate();
	}
}


void ChargeTime::setFormat(Format format)
{
	if (format != m_format)
	{
		m_format = format;
		setVisible(format != Format::Hidden);

		if (format != Format::Hidden)
		{
			minsLabel.setVisible(format == Format::Minimum || format == Format::Minutes || format == Format::Full);
			hoursLabel.setVisible(format == Format::Hours || format == Format::Full);

			value.setVisible(format != Format::Maximum);
			valueAdditional.setVisible(format == Format::Full);

			value.setWidth(format == Format::Full ? 106 : getWidth());
			hoursLabel.setWidth(format == Format::Full ? 50 : 104);
		}
	}
}

void ChargeTime::setTime(int h, int m)
{
	switch (m_format)
	{
	case Format::Full:
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", h);
		Unicode::snprintf(valueAdditionalBuffer, VALUEADDITIONAL_SIZE, "%02d", m);
		value.invalidate();
		valueAdditional.invalidate();
	} break;
	case Format::Hours:
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", h);
		value.invalidate();
	} break;
	case Format::Minutes:
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "%d", m);
		value.invalidate();
	} break;
	case Format::Minimum:
	{
		Unicode::snprintf(valueBuffer, VALUE_SIZE, "< 1");
//		Unicode::snprintf(valueAdditionalBuffer, VALUEADDITIONAL_SIZE, "%d", 1);
		value.invalidate();
//		valueAdditional.invalidate();
	} break;
	case Format::Maximum:
	default:
		break;
	}
}



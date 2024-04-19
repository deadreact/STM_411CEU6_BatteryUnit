#include <gui/containers/ChargeTime.hpp>
#include <texts/TextKeysAndLanguages.hpp>


constexpr static const uint8_t kZeroAlpha = 44;
constexpr static const uint32_t kBlue = 0xFF8FB5EF;
constexpr static const uint32_t kYellow = 0xFFFC9600;
constexpr static const uint32_t kRed = 0xFFFC0005;

constexpr static const uint32_t kLabelBlue = 0xFF687FCC;

ChargeTime::ChargeTime()
{
	applyFormat();
}

void ChargeTime::initialize()
{
    ChargeTimeBase::initialize();
}

void ChargeTime::setValue(int val)
{
	if (val != m_value)
	{
		const int absVal = val < 0 ? -val : val;
		int hours = absVal / 60;
		const int mins = absVal % 60;

		Format format;
		if (absVal > 9 * 60 + 30) {
			if (hours >= 99) {
				hours = 99;
			} else {
				hours = (absVal + 30)/ 60;
			}
			format = Format::Hours;
		} else if (absVal > 59) {
			format = Format::Full;
		} else if (absVal > 1) {
			format = Format::Minutes;
		} else if (absVal > 0) {
			format = Format::Minimum;
		} else { // 0
			format = Format::Hours;
			hours = 99;
		}

		setFormat(format);
		setTime(hours, mins);

		m_value = val;

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

		invalidate();
	}
}


void ChargeTime::setFormat(Format format)
{
	if (format != m_format)
	{
		m_format = format;
		applyFormat();
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
		value.invalidate();
	} break;
	default:
		break;
	}
}

void ChargeTime::applyFormat()
{
	minsLabel.setVisible(m_format == Format::Minimum || m_format == Format::Minutes || m_format == Format::Full);
	hoursLabel.setVisible(m_format == Format::Hours || m_format == Format::Full);

	valueAdditional.setVisible(m_format == Format::Full);

	value.setWidth(m_format == Format::Full ? 106 : getWidth());
	hoursLabel.setWidth(m_format == Format::Full ? 50 : 104);
}

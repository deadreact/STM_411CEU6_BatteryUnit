#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <cstring>
#include <algorithm>

DebugScreenView::DebugScreenView()
{
}

void DebugScreenView::setupScreen()
{
    DebugScreenViewBase::setupScreen();
}

DebugScreenView::~DebugScreenView()
{

}

void DebugScreenView::tearDownScreen()
{
    DebugScreenViewBase::tearDownScreen();
}

void DebugScreenView::setPower(int val)
{
	if (val != m_power)
	{
		float power = float(val) / 10000;
		touchgfx::Unicode::snprintfFloat(power_valueBuffer, POWER_VALUE_SIZE, "%.2f", power);
		if ((val < 0 && m_power >= 0) || (val > 0 && m_power <= 0) || val == 0)
		{
			label_charging.setVisible(val > 0);
			label_discharging.setVisible(val < 0);
			label_charging.invalidate();
			label_discharging.invalidate();
		}

		m_power = val;
		power_value.invalidate();
	}
}

void DebugScreenView::handleTickEvent()
{
	if (!SharedData::getData())
	{
		return;
	}
	const auto& data = *SharedData::getData();

	if (data.temperatureInv != temperatureInv)
	{
		temperatureInv = data.temperatureInv;
//		fan = data.fan;
		touchgfx::Unicode::snprintf(t_value_invtBuffer, T_VALUE_INVT_SIZE, "%d", data.temperatureInv);
		t_value_invt.invalidate();
	}

	if (data.temperatureCharg != temperatureCharg)
	{
		temperatureCharg = data.temperatureCharg;
		touchgfx::Unicode::snprintf(t_value_chargtBuffer, T_VALUE_CHARGT_SIZE, "%d", data.temperatureCharg);
		t_value_chargt.invalidate();
	}

	if (m_invState != data.invState) {
		m_invState = data.invState;
		updateInvState();
	}

	if (m_usbState != data.usbState) {
		m_usbState = data.usbState;
		icon_usb.setAlpha(m_usbState ? 255 : 110);
		icon_usb.invalidate();
	}

	if (m_fan != data.fan) {
		m_fan = data.fan;
		icon_fan.setAlpha(m_fan > 0 ? 205 + m_fan/2 : 110);
		icon_fan.invalidate();
	}

	if (data.chargerPlugged != icon_charge.isVisible())
	{
		icon_charge.setVisible(data.chargerPlugged);
		icon_charge.invalidate();
	}

	if (data.bms != m_bmsData)
	{
		//batteryProgressBar
		if (data.bms.soc != m_bmsData.soc)
		{
			m_bmsData.soc = data.bms.soc;
			batteryProgressBar.setProgress(m_bmsData.soc);
		}

		if (data.bms.current != m_bmsData.current)
		{
			m_bmsData.current = data.bms.current;
			touchgfx::Unicode::snprintfFloat(current_valueBuffer, CURRENT_VALUE_SIZE, "%.2f", static_cast<float>(m_bmsData.current) * 0.01f);
			current_value.invalidate();

			setPower(m_bmsData.current * m_bmsData.voltage);
		}

		if (data.bms.voltage != m_bmsData.voltage)
		{
			m_bmsData.voltage = data.bms.voltage;
			touchgfx::Unicode::snprintfFloat(voltage_valueBuffer, VOLTAGE_VALUE_SIZE, "%.2f", static_cast<float>(m_bmsData.voltage) * 0.01f);
//			voltageTextValue.resizeToCurrentText();
			voltage_value.invalidate();

			setPower(m_bmsData.current * m_bmsData.voltage);
		}

		if (data.bms.battery_box_temperature != m_bmsData.battery_box_temperature)
		{
			m_bmsData.battery_box_temperature = data.bms.battery_box_temperature;
			touchgfx::Unicode::snprintf(t_value_bmst1Buffer, T_VALUE_BMST1_SIZE, "%d", data.bms.battery_box_temperature);
			t_value_bmst1.invalidate();
		}
//
		if (data.bms.battery_temperature != m_bmsData.battery_temperature)
		{
			m_bmsData.battery_temperature = data.bms.battery_temperature;
			touchgfx::Unicode::snprintf(t_value_bmst2Buffer, T_VALUE_BMST2_SIZE, "%d", data.bms.battery_temperature);
			t_value_bmst2.invalidate();
		}

		if (data.bms.cellVoltage != m_bmsData.cellVoltage)
		{
			m_bmsData.cellVoltage = data.bms.cellVoltage;

			battery_cells.setNumberOfItems(m_bmsData.cellVoltage.size());
			for (int i = 0; i < m_bmsData.cellVoltage.size(); i++) {
				battery_cellsListItems[i].setIndex(i+1);
				battery_cellsListItems[i].setVoltage(m_bmsData.cellVoltage[i] * 0.001f);
			}

			colorizeCells();
			battery_cells.invalidateContent();
		}

		int chargeValue = data.bms.calcTimeRemain(data.bms.current);
		if (chargeValue != m_chargeTimeSec)
		{
			const int absVal = chargeValue < 0 ? -chargeValue : chargeValue;
			const int hours = absVal / SEC_IN_HOUR;
			const int mins = (absVal % SEC_IN_HOUR) / SEC_IN_MIN;
			touchgfx::Unicode::snprintf(time_valueBuffer1, TIME_VALUEBUFFER1_SIZE, "%02d", hours);
			touchgfx::Unicode::snprintf(time_valueBuffer2, TIME_VALUEBUFFER2_SIZE, "%02d", mins);
			m_chargeTimeSec = chargeValue;
			time_value.invalidate();
		}

		m_bmsData = data.bms;
	}

	bool isMajorError = data.errFlags & BMSErrorFlags::maskMajorErrors;
	if (isMajorError == batteryInfo.isVisible())
	{
		if (isMajorError)
		{
			m_bmsData = BatteryData();
		}
		batteryInfo.setVisible(!isMajorError);
		batteryInfo.invalidate();
	}

	if (data.errMsg != errMsg)
	{
		memset(errorLabelBuffer,0, ERRORLABEL_SIZE * 2);

		const char* msgIt = errMsg.cbegin();
		int i = 0;
		for (; *msgIt != '\0'; i++) {
			errorLabelBuffer[i] = *(msgIt++);
		}

		errMsg = data.errMsg;

		errorLabelBuffer[i] = '\n';
		++i;
		msgIt = errMsg.cbegin();
		for (; *msgIt != '\0'; i++) {
			errorLabelBuffer[i] = *(msgIt++);
		}

		errorLabel.invalidate();
	}

	if (m_invState == InverterState::Intermediate)
	{
		m_invAnimation.handleTickEvent();
	}
}

void DebugScreenView::colorizeCells()
{
	if (!m_bmsData.cellVoltage.empty())
	{
		uint16_t max = m_bmsData.cellVoltage[0];
		uint16_t min = m_bmsData.cellVoltage[0];
		uint8_t maxIndex = 0;
		uint8_t minIndex = 0;

		for (int i = 1; i < m_bmsData.cellVoltage.size(); i++) {
			if (m_bmsData.cellVoltage[i] > max) {
				max = m_bmsData.cellVoltage[i];
				maxIndex = i;
			} else if (m_bmsData.cellVoltage[i] < min) {
				min = m_bmsData.cellVoltage[i];
				minIndex = i;
			}
		}
		for (int i = 0; i < m_bmsData.cellVoltage.size(); i++) {
			battery_cellsListItems[i].setMarker(i == maxIndex ? CellMarker::Max : (i == minIndex ? CellMarker::Min : CellMarker::Average));
		}
	}
}


void DebugScreenView::updateInvState()
{
    icon_inv.setVisible(m_invState != InverterState::Off);

    if (m_invState == InverterState::On)
    {
        icon_inv.setAlpha(0xff);
    }
    icon_inv.invalidate();
}



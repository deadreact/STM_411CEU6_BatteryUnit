#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <cstring>
#include <algorithm>

DebugScreenView::DebugScreenView()
{
}

void DebugScreenView::setupScreen()
{
    DebugScreenViewBase::setupScreen();

    container_temperature_bms1.setTitle(T_TITLE_BMS_T1);
    container_temperature_bms2.setTitle(T_TITLE_BMS_T2);
    container_temperature_charg.setTitle(T_TITLE_CHARG_T);
    container_temperature_inv.setTitle(T_TITLE_INV_T);
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
		power_value.resizeToCurrentText();
		power_value.setX((container_power.getWidth() - power_value.getWidth() - power_units.getWidth() + 1)/2);
		power_units.setX(power_value.getX() + power_value.getWidth() + 1);

		container_power.invalidate();


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

	container_temperature_inv.setValue(data.temperatureInv);
	container_temperature_charg.setValue(data.temperatureCharg);

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
			current_value.resizeToCurrentText();
			current_value.setX((container_current.getWidth() - current_value.getWidth() - current_units.getWidth() + 1)/2);
			current_units.setX(current_value.getX() + current_value.getWidth() + 1);

			container_current.invalidate();

			setPower(m_bmsData.current * m_bmsData.voltage);
		}

		if (data.bms.voltage != m_bmsData.voltage)
		{
			m_bmsData.voltage = data.bms.voltage;
			touchgfx::Unicode::snprintfFloat(voltage_valueBuffer, VOLTAGE_VALUE_SIZE, "%.2f", static_cast<float>(m_bmsData.voltage) * 0.01f);
			voltage_value.resizeToCurrentText();
			voltage_value.setX((container_voltage.getWidth() - voltage_value.getWidth() - voltage_units.getWidth() + 1)/2);
			voltage_units.setX(voltage_value.getX() + voltage_value.getWidth() + 1);

			container_voltage.invalidate();

			setPower(m_bmsData.current * m_bmsData.voltage);
		}

		if (data.bms.battery_box_temperature != m_bmsData.battery_box_temperature)
		{
			m_bmsData.battery_box_temperature = data.bms.battery_box_temperature;
			container_temperature_bms1.setValue(m_bmsData.battery_box_temperature);
		}
//
		if (data.bms.battery_temperature != m_bmsData.battery_temperature)
		{
			m_bmsData.battery_temperature = data.bms.battery_temperature;
			container_temperature_bms2.setValue(m_bmsData.battery_temperature);
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



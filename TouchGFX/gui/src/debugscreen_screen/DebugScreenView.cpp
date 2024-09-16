#include <gui/debugscreen_screen/DebugScreenView.hpp>
#include <cstring>
#include <algorithm>

DebugScreenView::DebugScreenView()
{
}

void DebugScreenView::setupScreen()
{
    DebugScreenViewBase::setupScreen();

    for (int i = 0; i < BatteryData::kMaxCellCount; ++i)
    {
    	cell[i].initialize();
    	cell[i].setIndex(i+1);
    	cell[i].setVisible(false);
    	batteryCellInfo.add(cell[i]);
    }

    __background.setColor(0xff11212a);
}

DebugScreenView::~DebugScreenView()
{
	delete m_settingsPopUp;
}

void DebugScreenView::tearDownScreen()
{
    DebugScreenViewBase::tearDownScreen();
}

Settings* DebugScreenView::takeSettings()
{
	if (m_settingsPopUp)
	{
		remove(*m_settingsPopUp);
		auto tmp = m_settingsPopUp;
		m_settingsPopUp = nullptr;
		return tmp;
	}
	return nullptr;
}


void DebugScreenView::giveSettings(Settings* settings)
{
	if (settings)
	{
		m_settingsPopUp = settings;
		settings->setXY(40,  40);
		add(*settings);
	}
}

void DebugScreenView::handleTickEvent()
{
	const auto& data = SharedData::getData();

	if (data.temperature != temperature || data.fan != fan)
	{
		temperature = data.temperature;
		fan = data.fan;
		touchgfx::Unicode::snprintf(temperatureFanBuffer1, TEMPERATUREFANBUFFER1_SIZE, "%d", (int)data.temperature);
		touchgfx::Unicode::snprintf(temperatureFanBuffer2, TEMPERATUREFANBUFFER2_SIZE, "%d", data.fan);
		temperatureFan.invalidate();
	}

	if (data.bms != m_bmsData)
	{
		if (data.bms.soc != m_bmsData.soc)
		{
			m_bmsData.soc = data.bms.soc;
			capacityValue.setValue(m_bmsData.soc);
			touchgfx::Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", m_bmsData.soc);
//			capacityTextValue.resizeToCurrentText();
			capacityTextValue.invalidate();
		}

		if (data.bms.current != m_bmsData.current)
		{
			m_bmsData.current = data.bms.current;
			touchgfx::Unicode::snprintfFloat(currentTextValueBuffer, CURRENTTEXTVALUE_SIZE, "%.2f", static_cast<float>(m_bmsData.current) * 0.01f);
//			currentTextValue.resizeToCurrentText();
			currentTextValue.invalidate();
		}

		if (data.bms.voltage != m_bmsData.voltage)
		{
			m_bmsData.voltage = data.bms.voltage;
			touchgfx::Unicode::snprintfFloat(voltageTextValueBuffer, VOLTAGETEXTVALUE_SIZE, "%.2f", static_cast<float>(m_bmsData.voltage) * 0.01f);
//			voltageTextValue.resizeToCurrentText();
			voltageTextValue.invalidate();
		}

		if (data.bms.battery_box_temperature != m_bmsData.battery_box_temperature)
		{
			m_bmsData.battery_box_temperature = data.bms.battery_box_temperature;
			touchgfx::Unicode::snprintf(temperatureValuesBuffer1, TEMPERATUREVALUESBUFFER1_SIZE, "%d", data.bms.battery_box_temperature);
			temperatureValues.invalidate();
		}

		if (data.bms.battery_temperature != m_bmsData.battery_temperature)
		{
			m_bmsData.battery_temperature = data.bms.battery_temperature;
			touchgfx::Unicode::snprintf(temperatureValuesBuffer2, TEMPERATUREVALUESBUFFER2_SIZE, "%d", data.bms.battery_temperature);
			temperatureValues.invalidate();
		}

		if (data.bms.cellVoltage != m_bmsData.cellVoltage)
		{
			m_bmsData.cellVoltage = data.bms.cellVoltage;
			int i = 0;
			for (; i < m_bmsData.cellVoltage.size(); i++) {
				cell[i].setVisible(true);
				cell[i].setVoltage(m_bmsData.cellVoltage[i] * 0.001f);
			}
			for (; i < BatteryData::kMaxCellCount; i++) {
				cell[i].setVisible(false);
			}
			colorizeCells();
			batteryCellInfo.invalidateContent();
		}
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
			cell[i].setMarker(i == maxIndex ? CellMarker::Max : (i == minIndex ? CellMarker::Min : CellMarker::Average));
		}
	}
}



#include <gui/idlescreen_screen/IdleScreenView.hpp>
#include <cstring>

IdleScreenView::IdleScreenView()
{
	memset(errMsg, 0, sizeof(errMsg));
}

void IdleScreenView::setupScreen()
{
    IdleScreenViewBase::setupScreen();

    for (int i = 0; i < BatteryData::kMaxCellCount; ++i)
    {
    	cell[i].initialize();
    	cell[i].setIndex(i+1);
    	cell[i].setVisible(false);
    	batteryCellInfo.add(cell[i]);
    }
}

void IdleScreenView::tearDownScreen()
{
    IdleScreenViewBase::tearDownScreen();
}

void IdleScreenView::handleTickEvent()
{
    if (SharedData::getProcessId() != ProcessId::Idle)
    {
    	return;
    }
	const auto& data = SharedData::getData<ProcessId::Idle>();

	if (data.bms != m_bmsData)
	{
		if (data.bms.soc != m_bmsData.soc)
		{
			m_bmsData.soc = data.bms.soc;
			capacityValue.setValue(m_bmsData.soc);
			Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", m_bmsData.soc);
//			capacityTextValue.resizeToCurrentText();
			capacityTextValue.invalidate();
		}

		if (data.bms.current != m_bmsData.current)
		{
			m_bmsData.current = data.bms.current;
			Unicode::snprintfFloat(currentTextValueBuffer, CURRENTTEXTVALUE_SIZE, "%.2f", (float)m_bmsData.current * 0.01f);
//			currentTextValue.resizeToCurrentText();
			currentTextValue.invalidate();
		}

		if (data.bms.voltage != m_bmsData.voltage)
		{
			m_bmsData.voltage = data.bms.voltage;
			Unicode::snprintfFloat(voltageTextValueBuffer, VOLTAGETEXTVALUE_SIZE, "%.2f", (float)m_bmsData.voltage * 0.01f);
//			voltageTextValue.resizeToCurrentText();
			voltageTextValue.invalidate();
		}

		if (data.bms.cellCount != m_bmsData.cellCount)
		{
			m_bmsData.cellCount = data.bms.cellCount;
			memcpy(m_bmsData.cellVoltage, data.bms.cellVoltage, sizeof(uint16_t)*m_bmsData.cellCount);
			int i = 0;
			for (; i < m_bmsData.cellCount; i++) {
				cell[i].setVisible(true);
				cell[i].setVoltage(m_bmsData.cellVoltage[i] * 0.001f);
			}
			for (; i < BatteryData::kMaxCellCount; i++) {
				cell[i].setVisible(false);
			}
			batteryCellInfo.invalidateContent();
		}
		else if (memcmp(m_bmsData.cellVoltage, data.bms.cellVoltage, sizeof(uint16_t)*m_bmsData.cellCount) != 0)
		{
			memcpy(m_bmsData.cellVoltage, data.bms.cellVoltage, sizeof(uint16_t)*m_bmsData.cellCount);
			for (int i = 0; i < m_bmsData.cellCount; i++) {
				cell[i].setVoltage(m_bmsData.cellVoltage[i] * 0.001f);
			}
		}
	}

	bool isMajorError = data.bmsErrFlags & BMSErrorFlags::maskMajorErrors;
	if (isMajorError == batteryInfo.isVisible())
	{
		if (isMajorError)
		{
			m_bmsData = BatteryData();
		}
		batteryInfo.setVisible(!isMajorError);
		batteryInfo.invalidate();
	}

	if (data.bmsErrMsg != (const char*)errMsg)
	{
		memset(errorLabelBuffer,0, ERRORLABEL_SIZE * 2);

		const char* msgIt = errMsg;
		int i = 0;
		for (; *msgIt != '\0'; i++) {
			errorLabelBuffer[i] = *(msgIt++);
		}

		int bytesCpy = std::min(data.bmsErrMsg.size(), sizeof(errMsg) - 1);
		memcpy(errMsg, data.bmsErrMsg.c_str(), bytesCpy);
		errMsg[bytesCpy] = '\0';

		errorLabelBuffer[i] = '\n';
		++i;
		msgIt = errMsg;
		for (; *msgIt != '\0'; i++) {
			errorLabelBuffer[i] = *(msgIt++);
		}

		errorLabel.invalidate();
	}
}

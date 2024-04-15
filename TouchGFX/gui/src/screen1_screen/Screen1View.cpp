#include <gui/screen1_screen/Screen1View.hpp>
#include <shared_data.h>

namespace
{



}


Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    if (SharedData::getProcessId() != ProcessId::Idle)
	{
		return;
	}
	const auto& data = SharedData::getData<ProcessId::Idle>();
    updateBatteryData(data.bms);
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
//    m_bmsData = BatteryData();
//    m_chargeTimeMins = -1;
//    m_isBMSError = false;

}

void Screen1View::handleTickEvent()
{
    if (SharedData::getProcessId() == ProcessId::Startup)
    {
//        auto value = SharedData::getData<ProcessId::Startup>().timeLeftToStandby/70;
//        capacityValue.setValue(value);
    }
    if (SharedData::getProcessId() != ProcessId::Idle)
	{
		return;
	}
	const auto& data = SharedData::getData<ProcessId::Idle>();

	bool isMajorError = data.bmsErrFlags & BMSErrorFlags::maskMajorErrors;
	if (m_isBMSError != isMajorError)
	{
		warning.setVisible(isMajorError);
		warning.invalidate();
		m_isBMSError = isMajorError;
	}
	warning.handleTickEvent();

	if (data.bms != m_bmsData)
	{
		updateBatteryData(data.bms);
	}
}

void Screen1View::setWatts(int val)
{
	ioValue.setValue((float)val * 0.0001f);
}

void Screen1View::updateBatteryData(const BatteryData& data)
{
	if (data.isValid())
	{
		if (data.soc != m_bmsData.soc)
		{
			capacityContainer.setValue(data.soc);
			capacityContainerLarge.setValue(data.soc);
		}

		if (data.current != m_bmsData.current)
		{
			setWatts(data.current * data.voltage);

			if (data.current == 0) {
				capacityContainer.setVisible(false);
				capacityContainerLarge.setVisible(true);
				invalidate();
			} else if (m_bmsData.current == 0) {
				capacityContainer.setVisible(true);
				capacityContainerLarge.setVisible(false);
				invalidate();
			}
		}
		else if (data.voltage != m_bmsData.voltage)
		{
			setWatts(data.current * data.voltage);
		}

		int chargeValue = data.calcTimeRemain();
		if (chargeValue != m_chargeTimeMins)
		{
			chargeTimeContainer.setValue(chargeValue);
			m_chargeTimeMins = chargeValue;
		}
	}

	content.setVisible(data.isValid());

	m_bmsData = data;
}



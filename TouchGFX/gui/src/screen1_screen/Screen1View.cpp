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

    iValue.setIO(IOValue::In);
    oValue.setIO(IOValue::Out);
    iValue.setValue(0);
    oValue.setValue(0);

}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{
    if (SharedData::getProcessId() == ProcessId::Startup)
    {
        auto value = SharedData::getData<ProcessId::Startup>().timeLeftToStandby/70;
        capacityValue.setValue(value);
    }
    if (SharedData::getProcessId() != ProcessId::Idle)
	{
		return;
	}
	const auto& data = SharedData::getData<ProcessId::Idle>();

	if (data.bms != m_bmsData)
	{
		if (data.bms.capacityLevel != m_bmsData.capacityLevel)
		{
			capacityValue.setValue(data.bms.capacityLevel);
			Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", data.bms.capacityLevel);
			capacityTextValue.invalidate();
		}

		if (data.bms.current != m_bmsData.current)
		{
//			capacityValue.setValue(data.bms.capacityLevel);
//			Unicode::snprintf(capacityTextValueBuffer, CAPACITYTEXTVALUE_SIZE, "%d", data.bms.capacityLevel);
//			capacityTextValue.invalidate();
			setWatts(data.bms.current * data.bms.voltage / 10000);
		}
		else if (data.bms.voltage != m_bmsData.voltage)
		{
			setWatts(data.bms.current * data.bms.voltage / 10000);
		}

		m_bmsData = data.bms;
	}
}

void Screen1View::setWatts(int val)
{
	oValue.setValue(val > 0 ? val : 0);
	iValue.setValue(val < 0 ? -val : 0);
}


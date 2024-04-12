#include <gui/screen1_screen/Screen1View.hpp>
#include <shared_data.h>


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

		m_bmsData = data.bms;
	}
}

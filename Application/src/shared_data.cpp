/*
 * program_data.cpp
 *
 *  Created on: Apr 6, 2024
 *      Author: deadreact
 */

#include <shared_data.h>

void ProcessData<ProcessId::Idle>::updateBatteryData(const BatteryData& batData)
{
	auto soc = bms.soc & 0x7f;
	auto warnMsg = bms.warningMsg;

	bms = batData;

	if (batData.soc == 0) {
		if (warnMsg & 1) {
			bms.warningMsg |= 1;
		} else {

		}
	} else if (batData.soc < 5) {
		bms.warningMsg |= 1;
	}

	if (bms.isValid()) {
		smoothedCurrent.set(batData.current);
	} else {
		smoothedCurrent.reset();
	}
	bms.current = smoothedCurrent.get();
}

void ProcessData<ProcessId::Idle>::setPowerModeState(PowerModeState state)
{
	if (powerModeState != state)
	{
		powerModeState = state;
		if (state == PowerModeState::WakedUp)
		{
			smoothedCurrent.reset();
			bms.current = 0;
		}
	}
}


SharedData& SharedData::get()
{
    static SharedData data;
    return data;
}

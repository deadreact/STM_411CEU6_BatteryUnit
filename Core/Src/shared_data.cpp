/*
 * shared_data.cpp
 *
 *  Created on: Oct 20, 2024
 *      Author: deadreact
 */

#include <shared_data.h>


void ProcessData::setPowerModeState(PowerModeState state)
{
	if (powerModeState != state)
	{
		powerModeState = state;
//		if (state == PowerModeState::WakedUp)
//		{
//			smoothedCurrent.reset();
//			bms.current = 0;
//		}
	}
}


SharedData& SharedData::get()
{
    static SharedData data;
    if (!data.processData)
    {
    	static const ProcessData processData;
    	data.processData = &processData;
    }
    return data;
}

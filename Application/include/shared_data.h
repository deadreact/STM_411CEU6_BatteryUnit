/*
 * program_data.h
 *
 *  Created on: Apr 6, 2024
 *      Author: deadreact
 */

#ifndef SHARED_DATA_H_
#define SHARED_DATA_H_

#include <common_defs.h>
#include <bms_data.h>
#include <handlers/inverter_handler.h>
#include <stdint.h>
#include <utils/revision_data.h>
#include <utils/stack_string.h>
#include <settings_data.h>

enum class PowerModeState: uint8_t
{
    Normal,
    StopRequested,
    StopReady,
    WakedUp
};


/**
 * ProcessData contains data from IdleProcess used in touchGFX
 */
struct ProcessData
{
//	struct Settings
//	{
//		enum class ScreenIndex
//		{
//			Main = 0,
//			Debug
//		};
//
//		bool active{false};
//		int chargePower{500};
//		ScreenIndex screen;
//		int brightnessPercent{100};
//
//		bool operator==(const Settings& other) const
//		{
//			return active == other.active
//				&& chargePower == other.chargePower
//				&& screen == other.screen
//				&& brightnessPercent == other.brightnessPercent;
//		}
//		bool operator!=(const Settings& other) const
//		{
//			return !operator==(other);
//		}
//	};

    ScreenId screenId{ScreenId::DefaultScreen};
    InverterState invState{InverterState::Off};
    bool usbState{false};
    bool chargerPlugged{false};

    float temperatureInv{0.f};
    float temperatureCharg{0.f};
//    int fan{0};

    RevisionData<BatteryData> bms;
    SmoothedValue smoothedCurrent;
    utils::stack_string errMsg;
    uint32_t errFlags{0};

    SettingsData settings;

    PowerModeState getPowerModeState() const { return powerModeState; }
    void setPowerModeState(PowerModeState state);
    void updateBatteryData(const BatteryData& batData);

private:
    PowerModeState powerModeState{PowerModeState::Normal};
};


/**
 * @brief      Wraps ProcessData to avoid writing to it
 */
class SharedData
{
private:
    SharedData() = default;

public:
    SharedData(const SharedData&) = delete;
    SharedData& operator=(const SharedData&) = delete;
    SharedData(SharedData&&) = delete;
    SharedData& operator=(SharedData&&) = delete;

    static SharedData& get();
    static const ProcessData& getData() { return *get().processData; }
    static void shareReadOnly(const ProcessData& data) { get().processData = &data; }
private:
    const ProcessData* processData { nullptr };
};

#endif /* SHARED_DATA_H_ */

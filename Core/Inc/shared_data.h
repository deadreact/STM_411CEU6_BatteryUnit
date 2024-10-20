/*
 * shared_data.h
 *
 *  Created on: Oct 20, 2024
 *      Author: deadreact
 */

#ifndef INC_SHARED_DATA_H_
#define INC_SHARED_DATA_H_

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
    ScreenId screenId{ScreenId::DefaultScreen};
//    InverterState invState{InverterState::Off};
    bool usbState{false};
    bool chargerPlugged{false};

    int16_t temperatureInv{0};
    int16_t temperatureCharg{0};
//    int fan{0};

//    RevisionData<BatteryData> bms;
//    SmoothedValue smoothedCurrent;
//    std::string errMsg;
    uint32_t errFlags{0};

    SettingsData settings;

    PowerModeState getPowerModeState() const { return powerModeState; }
    void setPowerModeState(PowerModeState state);

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

#endif /* INC_SHARED_DATA_H_ */

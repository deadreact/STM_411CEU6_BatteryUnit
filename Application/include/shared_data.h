/*
 * program_data.h
 *
 *  Created on: Apr 6, 2024
 *      Author: deadreact
 */

#ifndef SHARED_DATA_H_
#define SHARED_DATA_H_

#include <bms/bms_data.h>
#include <inverter/inverter_handler.h>
#include <stdint.h>
#include <utils/revision_data.h>
#include <utils/stack_string.h>

enum class PowerModeState: uint8_t
{
    Normal,
    StopRequested,
    StopReady,
    WakedUp
};


/**
 * @brief      Process data/state is readonly available from everywhere
 */
struct ProcessData
{
    int screenId{1};
    InverterState invState{InverterState::Off};
    bool usbState{false};
    bool chargerPlugged{false};

    RevisionData<BatteryData> bms;
    SmoothedValue smoothedCurrent;
    utils::stack_string errMsg;
    uint32_t errFlags{0};

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

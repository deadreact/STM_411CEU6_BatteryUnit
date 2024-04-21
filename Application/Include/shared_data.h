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
#include <string>
#include <application_utils.h>

enum class PowerModeState: uint8_t
{
    Normal,
    StopRequested,
    StopReady,
    WakedUp
};

enum class ProcessId: uint8_t
{
    Invalid,
    Startup,
    Sleep,
    Idle
};

template <ProcessId id>
struct ProcessData;

template <>
struct ProcessData<ProcessId::Startup>
{
    int timeLeftToStandby{0};
};

template <>
struct ProcessData<ProcessId::Sleep> {};

template <>
struct ProcessData<ProcessId::Idle>
{
    int screenBrightness {500};
    int analog1{0};
    int analog2{0};

    int screenId{1};
    PowerModeState powerModeState{PowerModeState::Normal};
    InverterState invState{InverterState::Off};
    bool usbState{false};

    RevisionData<BatteryData> bms;
    SmoothedValue smoothedCurrent;
    std::string errMsg;
    uint32_t errFlags{0};
};


// --------------------------------------------------------------
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

    template <ProcessId id>
    void setProcessId(const ProcessData<id>* data);

    static ProcessId getProcessId() { return get().processId; }
    template <ProcessId id>
    static const ProcessData<id>& getData() { return *static_cast<const ProcessData<id>*>(get().processData); }
private:
    ProcessId processId {ProcessId::Invalid};
    const void* processData { nullptr };
};


// -----------------------------------
template <ProcessId id>
void SharedData::setProcessId(const ProcessData<id>* data)
{
    if (processId != id)
    {
        processId = id;
        processData = data;
    }
}

#endif /* SHARED_DATA_H_ */

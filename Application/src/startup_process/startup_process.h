/*
 * startup_process.h
 *
 *  Created on: Apr 5, 2024
 *      Author: deadreact
 */

#ifndef STARTUP_PROCESS_STARTUP_PROCESS_H_
#define STARTUP_PROCESS_STARTUP_PROCESS_H_

#include <shared_data.h>
#include <stdint.h>
#include "../process.h"

class StartupProcess : public Process
{
public:
    virtual void Init() override;
    virtual void Deinit() override;
protected:
    virtual void Update() override;
private:
    ProcessData<ProcessId::Startup> data;
    uint32_t m_startTick;
};

#endif /* STARTUP_PROCESS_STARTUP_PROCESS_H_ */

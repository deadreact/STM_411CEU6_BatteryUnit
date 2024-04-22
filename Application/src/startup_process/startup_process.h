/*
 * startup_process.h
 *
 *  Created on: Apr 5, 2024
 *      Author: deadreact
 */

#ifndef STARTUP_PROCESS_STARTUP_PROCESS_H_
#define STARTUP_PROCESS_STARTUP_PROCESS_H_

#include <shared_data.h>
#include <application_utils.h>
#include "../process.h"

class StartupProcess : public Process
{
public:
    virtual void init() override;
    virtual void deinit() override;
protected:
    virtual void update() override;
private:
    ProcessData<ProcessId::Startup> data;
    CTimeout m_standbyTimeout{7000};
};

#endif /* STARTUP_PROCESS_STARTUP_PROCESS_H_ */

/*
 * idle_process.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef IDLE_PROCESS_IDLE_PROCESS_H_
#define IDLE_PROCESS_IDLE_PROCESS_H_

#include "process.h"

class IdleProcess : public Process
{
public:
	IdleProcess();
    virtual ~IdleProcess();

    virtual void init() override;
protected:
    virtual void update() override;
private:
    struct Impl;
    Impl* m_pimpl{nullptr};
};

#endif /* IDLE_PROCESS_IDLE_PROCESS_H_ */

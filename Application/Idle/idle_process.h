/*
 * idle_process.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef IDLE_IDLE_PROCESS_H_
#define IDLE_IDLE_PROCESS_H_

#include "../process.h"

class IdleProcess : public Process
{
public:
    virtual ~IdleProcess();

    virtual void Init() override;
protected:
    virtual void Update() override;
private:
    struct Impl;
    Impl* m_pimpl{nullptr};
};

#endif /* IDLE_IDLE_PROCESS_H_ */

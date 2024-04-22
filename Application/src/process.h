/*
 * process.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef PROCESS_H_
#define PROCESS_H_

#include <utils/timeout.h>

class Process
{
public:
	Process(uint16_t tickRate = 1): m_tickRate(tickRate) {}
    virtual ~Process() {}
    virtual void init() = 0;
    virtual void deinit() {}

    void run();
protected:
    virtual void update() = 0;

    bool m_isRunning{false};
    CTimeout m_tickRate;
private:
    static Process* sm_current;
};


#endif /* PROCESS_H_ */

/*
 * process.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef PROCESS_H_
#define PROCESS_H_

class Process
{
public:
    virtual ~Process() {}
    virtual void init() = 0;
    virtual void deinit() {}

    void run();
protected:
    virtual void update() = 0;

    bool m_isRunning{false};
    unsigned int m_tickRate{1};
private:
    static Process* sm_current;
};


#endif /* PROCESS_H_ */

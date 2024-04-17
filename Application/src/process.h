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
    virtual void Init() = 0;
    virtual void Deinit() {}

    void Run();
protected:
    virtual void Update() = 0;

    bool m_isRunning{false};
    unsigned int m_tickRate{1};
private:
    static Process* sm_current;
};


#endif /* PROCESS_H_ */

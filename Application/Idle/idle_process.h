/*
 * idle_process.h
 *
 *  Created on: Apr 7, 2024
 *      Author: deadreact
 */

#ifndef IDLE_IDLE_PROCESS_H_
#define IDLE_IDLE_PROCESS_H_

class IdleProcess
{
public:
	virtual ~IdleProcess();

	void Init();
	void Run();
private:
	struct Impl;
	Impl* m_pimpl{nullptr};
	bool m_isRunning{false};
};

#endif /* IDLE_IDLE_PROCESS_H_ */

/*
 * startup_process.h
 *
 *  Created on: Apr 5, 2024
 *      Author: deadreact
 */

#ifndef STARTUP_STARTUP_PROCESS_H_
#define STARTUP_STARTUP_PROCESS_H_

#include <stdint.h>

class StartupProcess {
public:
	StartupProcess();
	virtual ~StartupProcess();

	void Init();
	void Run();
private:
	bool m_isRunning {false};
	uint32_t m_startTick;
};

#endif /* STARTUP_STARTUP_PROCESS_H_ */

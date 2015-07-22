/*
 * CTaskManager.h
 *
 *  Created on: Jan 20, 2014
 *      Author: billy
 */



#ifndef CTASKMANAGER_H_
#define CTASKMANAGER_H_
#include "ITask.h"
#include "Lumen.h"
#include <queue>

class CTaskManager {
private:
	std::queue<ITask*> tasks;

public:
	bool to_run = false;
	void add_task(ITask* task);
	void remove_task(ITask* task);
	void run();
	uint8_t wait_more = 0;

};

extern CTaskManager TaskManager;

#endif /* CTASKMANAGER_H_ */

/*
 * TaskBluetooth.h
 *
 *  Created on: Jan 21, 2014
 *      Author: billy
 */

#ifndef TASKBLUETOOTH_H_
#define TASKBLUETOOTH_H_

#include "ITask.h"

class CTaskBluetooth: public ITask {
public:
	CTaskBluetooth();
	virtual ~CTaskBluetooth();

	void run();
	bool is_repeating();
};

extern CTaskBluetooth TaskBluetooth;

#endif /* TASKBLUETOOTH_H_ */

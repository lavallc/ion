/*
 * TaskLeds.h
 *
 *  Created on: Jan 21, 2014
 *      Author: billy
 */

#ifndef TASKLEDS_H_
#define TASKLEDS_H_

#include "ITask.h"
#include "LumenLeds.h"

class CTaskLeds: public ITask {
private:
	uint8_t red,green,blue;
	int direction;
public:


	void run();
	bool is_repeating();
};

extern CTaskLeds TaskLeds;

#endif /* TASKLEDS_H_ */

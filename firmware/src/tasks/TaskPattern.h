/*
 * TaskPattern.h
 *
 *  Created on: Jan 22, 2014
 *      Author: billy
 */

#ifndef TASKPATTERN_H_
#define TASKPATTERN_H_

#include "ITask.h"
#include <cstdint>

class CTaskPattern: public ITask {

public:
	uint16_t red = 0, green = 0, blue = 0;
	uint16_t direction = 1;

	void run();
	bool is_repeating();


};

extern CTaskPattern TaskPattern;
#endif /* TASKPATTERN_H_ */

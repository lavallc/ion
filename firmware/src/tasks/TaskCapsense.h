/*
 * TaskCapsense.h
 *
 *  Created on: Jan 23, 2014
 *      Author: billy
 */

#ifndef TASKCAPSENSE_H_
#define TASKCAPSENSE_H_
#include "ITask.h"
#include "Lumen.h"
class CTaskCapsense : public ITask {
private:
	bool capsense_touch_dispatched_already = false;
	bool prox_touch_on = false;
	bool knock_ended = false;
	uint8_t prox_touch_wait = 0;
	uint8_t capsense_held_time = 0;
public:

	void run();
	bool is_repeating();
};

extern CTaskCapsense TaskCapsense;

#endif /* TASKCAPSENSE_H_ */

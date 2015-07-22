/*
 * TaskSerial.h
 *
 *  Created on: May 28, 2014
 *      Author: eric
 */

#ifndef TASKSERIAL_H_
#define TASKSERIAL_H_
#include "ITask.h"

class CTaskSerial : public ITask {
private:

public:

	void run();
	bool is_repeating();
};

extern CTaskSerial TaskSerial;

#endif /* TASKSERIAL_H_ */

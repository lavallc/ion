/*
 * ITask.h
 *
 *  Created on: Jan 21, 2014
 *      Author: billy
 */

#ifndef ITASK_H_
#define ITASK_H_

class ITask {
public:
	virtual void run() = 0;
	virtual bool is_repeating() = 0;
};

#endif /* ITASK_H_ */

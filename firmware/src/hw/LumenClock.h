/*
 * LumenClock.h
 *
 *  Created on: Feb 17, 2014
 *      Author: ebarch
 */

#ifndef LUMENCLOCK_H_
#define LUMENCLOCK_H_

extern "C" {
#include "nrf.h"
}
class CLumenClock {
public:
	unsigned char get_seconds();
	unsigned char get_minutes();
	unsigned char get_hours_12();
	unsigned char get_hours_24();
	unsigned int get_uptime();
	bool is_pm();
	bool has_been_set();
	void set_clock(unsigned char sec, unsigned char min, unsigned char hour);
	void on_tick();
	volatile unsigned int _uptime = 0;	// uptime in seconds
private:
	bool _has_been_set =  false;
	volatile unsigned char _sec = 0;
	volatile unsigned char _min = 0;
	volatile unsigned char _hour = 0;	// stored in 24-hour format

};

extern CLumenClock LumenClock;

#endif /* LUMENCLOCK_H_ */

/*
 * LumenClock.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: ebarch
 */
#include "Serial.h"
#include "LumenClock.h"

CLumenClock LumenClock;


unsigned char CLumenClock::get_seconds() {
	return _sec;
}

unsigned char CLumenClock::get_minutes() {
	return _min;
}

unsigned char CLumenClock::get_hours_12() {
	if (_hour > 12)
		return _hour - 12;
	else
		return _hour;
}

unsigned char CLumenClock::get_hours_24() {
	return _hour;
}

unsigned int CLumenClock::get_uptime() {
	return _uptime;
}

bool CLumenClock::has_been_set() {
	return _has_been_set;
}

bool CLumenClock::is_pm() {
	return (_hour >= 12);
}


void CLumenClock::set_clock(unsigned char sec, unsigned char min, unsigned char hour) {
	_sec = sec;
	_min = min;
	_hour = hour;
	_has_been_set = true;
}

// this should be called once per second
void CLumenClock::on_tick() {
	_sec++;
	_uptime++;

	if (_sec > 59) {
		_sec = 0;
		_min++;
	}
	if (_min > 59) {
		_min = 0;
		_hour++;
	}
	if (_hour > 23) {
		_hour = 0;
	}
}

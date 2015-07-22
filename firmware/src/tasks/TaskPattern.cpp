/*
 * TaskPattern.cpp
 *
 *  Created on: Jan 22, 2014
 *      Author: billy
 */

#include "TaskPattern.h"
#include "Lumen.h"
#include "PatternManager.h"
#include "NotificationManager.h"

CTaskPattern TaskPattern;


void CTaskPattern::run() {
	//nrf_gpio_pin_set(kSunBoardDigPin2);
	if (NotificationManager.notification_is_active()) {
		NotificationManager.step();
	} else {
		PatternManager.step();
	}
	//nrf_gpio_pin_clear(kSunBoardDigPin2);
}

bool CTaskPattern::is_repeating() {
	return false;
}



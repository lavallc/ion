/*
 * TaskLeds.cpp
 *
 *  Created on: Jan 21, 2014
 *      Author: billy
 */

#include "TaskLeds.h"
#include "Lumen.h"

extern "C" {
	#include "app_util.h"
}

CTaskLeds TaskLeds;


void CTaskLeds::run() {
	if(!Lumen.radio_is_active && !Lumen.interrupts->adc_is_active) {

		CRITICAL_REGION_ENTER()
		LumenLeds.show();
		CRITICAL_REGION_EXIT()
	}
}

bool CTaskLeds::is_repeating() {
	return false;
}

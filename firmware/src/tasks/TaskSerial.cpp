/*
 * TaskSerial.cpp
 *
 *  Created on: May 28, 2014
 *      Author: eric
 */

#include "TaskSerial.h"
#include "Serial.h"
#include "Lumen.h"

CTaskSerial TaskSerial;


void CTaskSerial::run() {

	Serial.print_from_buffer();
}

bool CTaskSerial::is_repeating() {
	return false;
}

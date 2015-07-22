/*
 * TaskBluetooth.cpp
 *
 *  Created on: Jan 21, 2014
 *      Author: billy
 */

#include "TaskBluetooth.h"
#include "BluetoothLE.h"
#include "Lumen.h"
using namespace BLEpp;

CTaskBluetooth TaskBluetooth;

CTaskBluetooth::CTaskBluetooth() {
	// TODO Auto-generated constructor stub
}


void CTaskBluetooth::run() {
	// process events from bluetooth le such as writing packets, reading packets, dispatching new events
	Nrf51822BluetoothStack::_stack->process_event();
}

bool CTaskBluetooth::is_repeating() {
	return false;
}

CTaskBluetooth::~CTaskBluetooth() {
	// TODO Auto-generated destructor stub
}

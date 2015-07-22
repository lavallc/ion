/*
 * BluetoothManager.h
 *
 *  Created on: Jan 25, 2014
 *      Author: billy
 */

#ifndef BLUETOOTHMANAGER_H_
#define BLUETOOTHMANAGER_H_
#include <cstdint>
#include "BluetoothLE.h"
#include "ANCSHandler.h"
#include "PacketTypes.h"
#include "LumenService.h"

using namespace BLEpp;


class CBluetoothManager {
private:
	Nrf51822BluetoothStack stack;
	int counter= 0;
	string deviceName;
public:
	//startup and configuration
	void flip_radio_bit();
	void init();
	void build_lumen_service();
	void build_ancs_handler();
	void setup_radio_active_event();
	bool device_is_connected();
	void start_advertising();
	void rename_device(char * name_array, int length);
};

extern CBluetoothManager BluetoothManager;

#endif /* BLUETOOTHMANAGER_H_ */

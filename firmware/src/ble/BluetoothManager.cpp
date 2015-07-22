/*
 * BluetoothManager.cpp
 *
 *  Created on: Jan 25, 2014
 *      Author: billy
 */

#include "BluetoothManager.h"
#include "Lumen.h"
#include "LumenSettings.h"
#include "TaskManager.h"
#include "TaskBluetooth.h"
#include "TaskPattern.h"
#include "TaskSerial.h"
#include "TaskLeds.h"
#include "TaskCapsense.h"
#include "TaskSerial.h"
#include "FileHandler.h"

CBluetoothManager BluetoothManager;

void CBluetoothManager::flip_radio_bit() {
	uint8_t radio_notify;
	Nrf51822BluetoothStack::_stack->_radio_notify = radio_notify = ( Nrf51822BluetoothStack::_stack->_radio_notify + 1) % 2;
	Nrf51822BluetoothStack::_stack->_callback_radio(radio_notify == 1);
}

void CBluetoothManager::init() {

    // Set advertising parameters such as the device name and appearance.  These values will
    stack.setDeviceName("ion_fault");

    stack.setTxPowerLevel(4);

    //initialize soft device and setup stack parameters
    stack.init();

	stack.onConnect([&](uint16_t conn_handle) {
			//connected
			//Serial.println("Device Connected");
			//todo: post connected notification to taskmanager
		 })
		 .onDisconnect([&](uint16_t conn_handle) {
			//disconnected
			//Serial.println("Device Disconnected");
			//todo: post disconnected notification to taskmanager
			stack.startAdvertising();
		 });
	FileHandler.init();
	ANCSHandler.init();
	this->setup_radio_active_event();
	this->build_lumen_service();
	//initialize device settings
	LumenSettings.init();
	rename_device(LumenSettings.get_device_name(), LumenSettings.get_device_name_length());


    // Begin sending advertising packets over the air.
    
}

// is there a device connected to the lamp right now?
bool CBluetoothManager::device_is_connected() {
	return stack.connected();
}

void CBluetoothManager::start_advertising() {
	stack.startAdvertising();
}

void CBluetoothManager::rename_device(char * name_array, int length) {
	deviceName = string(name_array, length);
	stack.setDeviceName(deviceName);
	stack.rename();
}

void CBluetoothManager::build_lumen_service() {
	stack.addService(&LumenService);
}


void CBluetoothManager::setup_radio_active_event() {
	stack.onRadioNotificationInterrupt(1740, [&](bool radio_active){
		Lumen.radio_is_active = radio_active;
		if (radio_active) {
			nrf_gpio_pin_set(kSunBoardDigPin3);
			//We will restart timer2(led updater) after radio update.
			//This ensures the LED timing is matched to the radio update rate.
			Lumen.interrupts->stop();
			Lumen.interrupts->start_timer2();


		} else {
			nrf_gpio_pin_clear(kSunBoardDigPin3);
			Lumen.interrupts->stop_timer2();

			//Timers were just started and won't schedule anything until they expire.
			//Therefore, we will schedule events right now.
			nrf_delay_us(250);
			TaskManager.add_task(&TaskCapsense);	//See TaskManager, this doesn't work the same....
			//TaskManager.add_task(&TaskPattern);
			//TaskManager.add_task(&TaskLeds);
			//TaskManager.add_task(&TaskSerial);
			Lumen.interrupts->start();
		}
	});
}

// Project Lumen
// lava 2013


#include "BluetoothManager.h"
#include "Lumen.h"
#include "NotificationManager.h"
#include "PatternManager.h"

#include "TaskManager.h"
#include "TaskBluetooth.h"
#include "TaskLeds.h"
#include "TaskPattern.h"
#include "TaskCapsense.h"
#include "LumenSettings.h"



int main(void) {


	//initialize bluetooth / softdevice and dependents
	BluetoothManager.init();

	//initialize lumen hw layer
	Lumen.init();

	//initialize notification system
	NotificationManager.init();

	//initialize pattern system
	PatternManager.init();

	//lets add some initial tasks to the task manager
	TaskManager.add_task(&TaskCapsense);
	TaskManager.add_task(&TaskPattern);
	TaskManager.add_task(&TaskLeds);
	TaskManager.run();
	BluetoothManager.start_advertising();
	TaskManager.add_task(&TaskCapsense);
	TaskManager.add_task(&TaskPattern);
	TaskManager.add_task(&TaskLeds);
	//run the task manager forever
	while (1) {
		TaskManager.run();
	}
}


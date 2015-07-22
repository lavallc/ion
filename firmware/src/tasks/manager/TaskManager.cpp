/*
 * CTaskManager.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: billy
 */

#include "TaskManager.h"
#include "Lumen.h"
#include "TaskLeds.h"
#include "TaskCapsense.h"
#include "TaskPattern.h"
#include "TaskSerial.h"
#include "FileHandler.h"
#include "ANCSHandler.h"
#include "LumenSettings.h"
CTaskManager TaskManager;


void CTaskManager::add_task(ITask* task) {
	//this->tasks.push(task);
	to_run = true;
}

void CTaskManager::remove_task(ITask* task) {
//	this->tasks.remove(task);
}

void CTaskManager::run() {
	/*while(!this->tasks.empty()) {
		nrf_gpio_pin_set(kSunBoardDigPin2);
		//Pop first task and run it
		ITask *task = this->tasks.front();
		nrf_gpio_pin_clear(kSunBoardDigPin2);
		task->run();
		nrf_gpio_pin_set(kSunBoardDigPin2);
		this->tasks.pop();
		nrf_gpio_pin_clear(kSunBoardDigPin2);
		if(task->is_repeating()) this->add_task(task);

	}*/

	if (to_run) {
		if (!FileHandler.is_busy()) {
			if (wait_more == 0) {
				if (ANCSHandler.store_sys_settings) {
					ANCSHandler.store_sys_settings = false;
					LumenSettings.save_sys_settings();
				}
				nrf_gpio_pin_set(kSunBoardDigPin2);
				TaskCapsense.run();
				nrf_gpio_pin_clear(kSunBoardDigPin2);
				TaskPattern.run();
				nrf_gpio_pin_set(kSunBoardDigPin2);
				TaskLeds.run();
				nrf_gpio_pin_clear(kSunBoardDigPin2);
				TaskSerial.run();
			}
			else {
				wait_more--;
			}
		}
		else
			wait_more = 4;
		to_run = false;
	}


}



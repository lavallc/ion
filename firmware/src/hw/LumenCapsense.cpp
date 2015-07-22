//
//  LumenCapsense.cpp
//  firmware
//
//  Created by Billy Lindeman on 1/20/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#include "LumenCapsense.h"

CLumenCapsense LumenCapsense;

CLumenCapsense::CLumenCapsense() {
}

void CLumenCapsense::init() {
	uint8_t data[10];
	twi_master_init();

	lumen_capsense_ambient_setpoint = 200;

	mpr121_soft_reset();
	nrf_delay_us(100);
	mpr121_config_prox_touch_0();
	nrf_delay_us(100000);
	mpr121_read_register(MPR121_OOR_0_7, data);
}

void CLumenCapsense::add_on_touch_handler(LumenCapsenseEvent evt) {
	event_handlers.push_front(evt);
}

void CLumenCapsense::remove_on_touch_handler(LumenCapsenseEvent evt) {
	//TODO figure out how to remove std::function types from the list
	//(something about == not working)
	//	event_handlers.remove(evt);
}

bool CLumenCapsense::is_touched() {
	return mpr121_isTouched_electrode(0);
}

uint16_t CLumenCapsense::read() {
	uint16_t filtered_data;
	mpr121_read_electrode(0, &filtered_data);
	return filtered_data;
}

uint8_t CLumenCapsense::get_proximity(void) {
	//0 = Touching
	//255 = Out of Range
	uint8_t distance;
	uint16_t filtered_data = this->read();
	if (filtered_data < lumen_capsense_ambient_setpoint) {
		filtered_data = lumen_capsense_ambient_setpoint - filtered_data;
		if (filtered_data > 300) { //Touch event
			distance = 0;
		} else if (filtered_data > 100) { //Out of range of log algorithm, but almost touch
			distance = 1;
		} else {
			//linear brightness
			filtered_data = (uint16_t) (log10((float) filtered_data) * 127);
			distance = 255 - (uint8_t) filtered_data;
		}
	} else {
		distance = 255;
	}
    
	return distance;
}

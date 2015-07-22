/*
 * PatternHalo.cpp
 *
 *  Created on: Jan 30, 2014
 *      Author: billy
 */

#include "PatternHalo.h"
#include "sun_board.h"
#include "PatternHelpers.h"
#include "Serial.h"
#include <cmath>
CPatternHalo PatternHalo;

void CPatternHalo::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	index = 0;
	progress = 255;
	direction = 1; //down
	for (int i = 0; i < 10; i++) {
		values[i] = 0;
		values_next[i] = 0;
	}

	if (configs == NULL) {
			hue = 0;
			speed = 30;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		hue = configs->config_data[0].data;
		speed = configs->config_data[1].data;
		value = configs->config_data[2].data;
		saturation = configs->config_data[3].data;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
	Serial.debug_print("restart?");
}
void CPatternHalo::get_pattern_config(LumenMoodConfig * configs) {

}

void CPatternHalo::step() {
	if(progress >= 255) {
		//reset moving index and tween progress
		progress = 0;
		index += 1*direction;
		if(index > 9 || index < 0) {
			direction *= -1;
			index += 2*direction;
		}

		//lets copy PatternHelpers.value_next[0] into pattern
		for(int i=0; i<10; i++) {
			values[i] = values_next[i];
		}

		//then calculate pattern next
		for(int i=0; i<kSunBoardNumberOfLedsPerRay; i++) {
			int target = 0;
			if(i == index) target = value;
			if(i == index - direction) target = ((uint16_t)value * 2) / 5;
			if(i == index - 2*direction) target = value / 5;
			values_next[i] = target;
		}
	}

	//lerp from pattern to PatternHelpers.value_next[0]
	for(int i = 0; i<kSunBoardNumberOfLedsPerRay; i++) {
		int b;
		//Commented out is how to properly lerp a linearized brightness.
		//if (i == index)
			//if (progress < 30)
			//	b = PatternHelpers.lerp(PatternHelpers.value[0][i], PatternHelpers.value_next[0][i], 50 * log10((float)progress/3.0 + 1.0));
			//else
			//	b = PatternHelpers.lerp(PatternHelpers.value[0][i], PatternHelpers.value_next[0][i], 2 * progress / 3 + 30);
		//else
			b = PatternHelpers.lerp(values[i], values_next[i], progress);
		Lumen.leds->set_level_hsv(i, hue, saturation, b);
	}

	progress += speed;
}

void CPatternHalo::set_pattern_hue(uint16_t new_hue) {
	hue = new_hue;
}

void CPatternHalo::set_pattern_speed(uint16_t new_speed) {
	speed = new_speed;
}

uint32_t CPatternHalo::get_pattern_id() {
	return 0x000000005;
}

void CPatternHalo::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternHaloConfigSetHue: this->set_pattern_hue((uint16_t)data); break;
		case kPatternHaloConfigSetSpeed: this->set_pattern_speed((uint16_t)data); break;
	}
}


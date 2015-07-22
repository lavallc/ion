



/*
 * PatternRainbow.cpp
 *
 *  Created on: Jan 31 2014
 *      Author: Salemancer
 *      Pattern Creator: Billy
 */

#include "PatternRainbow.h"
#include "PatternHelpers.h"
#include <cmath>
CPatternRainbow PatternRainbow;

void CPatternRainbow::start(LumenMoodConfig * configs, bool restore_to_defaults) {

	if (configs == NULL) {
		PatternHelpers.speed = 1;
		is_reverse = 1; //means 0
		value = 153;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		set_pattern_speed(configs->config_data[0].data);
		value = configs->config_data[1].data;
		set_pattern_reverse(configs->config_data[2].data);
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternRainbow::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternRainbowConfigSetSpeed;
	configs->config_data[0].data = PatternHelpers.speed * 10;
	configs->config_data[1].config_id = kPatternRainbowConfigSetBrightness;
	configs->config_data[1].data = value;
	configs->config_data[2].config_id = kPatternRainbowConfigSetReverse;
	if (is_reverse == 1)
		configs->config_data[2].data = 0;
	else
		configs->config_data[2].data = 1;
}

void CPatternRainbow::step() {

	hue += (is_reverse * PatternHelpers.speed);
	if (hue < 0) hue = hue + 360;
	if (hue > 360) hue = hue - 360;
	for(int j=0; j<kSunBoardNumberOfLedsPerRay; j++) {
		Lumen.leds->set_level_linear_hsv(j, ((uint16_t)hue + 360) + (10*j), 255, value);
	}

}

void CPatternRainbow::set_pattern_speed(int new_speed) {
	new_speed = new_speed / 10;
	PatternHelpers.speed= new_speed; //set as 0 - 15
}

void CPatternRainbow::set_pattern_brightness(int new_brightness) {
	this->value= new_brightness;
}

void CPatternRainbow::set_pattern_reverse(int set_reverse) {

	if (set_reverse) {
		this->is_reverse = -1;
	}
	else {
		this->is_reverse = 1;
	}
}

uint32_t CPatternRainbow::get_pattern_id() {
	return 0x0000000C;
}

void CPatternRainbow::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternRainbowConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternRainbowConfigSetBrightness: this->set_pattern_brightness((int)data); break;
		case kPatternRainbowConfigSetReverse: this->set_pattern_reverse((int)data); break;
		break;
	}
}

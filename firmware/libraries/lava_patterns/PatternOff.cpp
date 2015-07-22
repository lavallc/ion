/*
 * PatternOff.cpp
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#include "PatternOff.h"
#include "PatternHelpers.h"

CPatternOff PatternOff;

void CPatternOff::start(LumenMoodConfig * configs, bool restore_to_defaults) {


	if (configs == NULL) {
		already_set = false;
		finished = true;
		last_value = 0;
		value = 0;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		already_set = false;
		finished = true;
		last_value = 0;
		value = 0;
	}
	else {
		PatternHelpers.progress = 255;
		transition();
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternOff::get_pattern_config(LumenMoodConfig * configs) {

}
void CPatternOff::step() {
	if(!already_set) {
		already_set = true;
		Lumen.leds->set_solid_rgb(0,0,0);
	}
	if (!finished) {
		PatternHelpers.progress += 5;
		this->transition();
	}
}

uint32_t CPatternOff::get_pattern_id() {
	return 0x00000001;
}

void CPatternOff::transition() {

	int v= PatternHelpers.lerp(last_value, value, PatternHelpers.progress);
	Lumen.leds->set_level_hsv(0,0, 0, 0);
	Lumen.leds->set_level_hsv(1,0, 0, 0);
	Lumen.leds->set_level_hsv(2,0, 0, 0);
	Lumen.leds->set_level_hsv(3,0, 0, 0);
	Lumen.leds->set_level_hsv(4,0, 0, 0);
	Lumen.leds->set_level_hsv(5,0, 0, 0);
	Lumen.leds->set_level_hsv(6,0, 0, 0);
	Lumen.leds->set_level_hsv(7,0, 0, v * 0.6);
	Lumen.leds->set_level_hsv(8,0, 0, v * 0.75);
	Lumen.leds->set_level_hsv(9,0, 0, v);

	if (PatternHelpers.progress == 255) {
		last_value = value;
		finished = true;
	}
}
void CPatternOff::set_pattern_lowglow(bool enabled) {
		low_glow_enabled = enabled;
		PatternHelpers.progress = 5;
		if (low_glow_enabled)
			value = 255;
		else
			value = 0;
		this->transition();
		finished = false;
}

void CPatternOff::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternOffLowGlow: this->set_pattern_lowglow((bool)data); break;

	}
}

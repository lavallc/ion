/*
 * PatternStrobe.cpp
 *
 *  Created on: Jan 31, 2014
 *      Author: barch
 */

#include "PatternStrobe.h"
#include "PatternHelpers.h"
extern "C" {
	#include "app_util.h"
}
CPatternStrobe PatternStrobe;

void CPatternStrobe::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	light_on = false;
	count = 0;
	changes_ready = 0;
	if (configs == NULL) {
		PatternHelpers.hue[0][0] = 0;
		PatternHelpers.saturation[0][0] = 255;
		stay_off = 7;
		color_enabled = false;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		set_pattern_speed(configs->config_data[0].data);
		PatternHelpers.hue[0][0] = configs->config_data[1].data;
		PatternHelpers.saturation[0][0] = configs->config_data[2].data;
		color_enabled = configs->config_data[3].data;


	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternStrobe::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternStrobeConfigSetSpeed;
	if (stay_off == 2)
		configs->config_data[0].data = 9;
	else if (stay_off == 3)
		configs->config_data[0].data = 24;
	else if (stay_off == 4)
		configs->config_data[0].data= 49;
	else if (stay_off == 5)
		configs->config_data[0].data = 74;
	else if (stay_off == 6)
		configs->config_data[0].data = 104;
	else
		configs->config_data[0].data = (stay_off * 5 + 70);

	configs->config_data[0].data = 255 - configs->config_data[0].data;
	configs->config_data[1].config_id = kPatternStrobeConfigSetHue;
	configs->config_data[1].data = PatternHelpers.hue[0][0];
	configs->config_data[2].config_id = kPatternStrobeConfigSetSaturation;
	configs->config_data[2].data = PatternHelpers.saturation[0][0];
	configs->config_data[3].config_id = kPatternStrobeConfigSetColorEnabled;
	configs->config_data[3].data = color_enabled;
}
void CPatternStrobe::toggle() {
	if (light_on) {
		Lumen.leds->set_solid_hsv(0, 0, 0);
	}
	else if (!color_enabled)
		Lumen.leds->set_solid_hsv(0, 0, 255);
	else
		Lumen.leds->set_solid_hsv(PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], 255);

	// flip it
	light_on = !light_on;
}

void CPatternStrobe::step() {
	count++;
	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] = PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}
	if (count == stay_off) {
		this->toggle();
		CRITICAL_REGION_ENTER()
		LumenLeds.show();
		CRITICAL_REGION_EXIT()
		count = 0;
		nrf_delay_us(5000);
		this->toggle();
		CRITICAL_REGION_ENTER()
		LumenLeds.show();
		CRITICAL_REGION_EXIT()
	}
}

void CPatternStrobe::set_pattern_speed(int new_speed) {
	count = 0;

	//if (delay_count > 25) delay_count = 25;
	//delay_count = 25 - delay_count;
	//this->stay_off = delay_count;
	new_speed = 255 - new_speed;
		if (new_speed < 10)
			stay_off = 2;
		else if (new_speed < 25)
			stay_off = 3;
		else if (new_speed < 50)
			stay_off= 4;
		else if (new_speed < 75)
			stay_off = 5;
		else if (new_speed < 104)
			stay_off = 6;
		else
			stay_off = ((new_speed - 70)/ 5);
}

void CPatternStrobe::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternStrobe::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternStrobe::set_pattern_color_enabled(bool enabled) {
	this->color_enabled = enabled;
}

uint32_t CPatternStrobe::get_pattern_id() {
	return 0x0000000A;
}

void CPatternStrobe::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternStrobeConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternStrobeConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternStrobeConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternStrobeConfigSetColorEnabled: this->set_pattern_color_enabled((bool)data); break;
	}
}

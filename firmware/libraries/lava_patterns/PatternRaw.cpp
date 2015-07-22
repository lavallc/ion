
/*
 * PatternRaw.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: Salemancer
 *      Pattern Creator: Salemancer
 */
#include "PatternHelpers.h"
#include "PatternRaw.h"

CPatternRaw PatternRaw;



void CPatternRaw::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	led_id_set  = false;
	bank_id_set = false;
	trigger_refresh = false;
	PatternHelpers.erase_all();
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 4; j++) {
			Lumen.leds->set_pixel_rgb(j, i,PatternHelpers.hue[j][i], PatternHelpers.saturation[j][i], PatternHelpers.value[j][i]);
		}
	}
	if (configs == NULL) {

	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		//Fill collecting saved configs.
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}

}
void CPatternRaw::get_pattern_config(LumenMoodConfig * configs){

	//Todo return configs
}

void CPatternRaw::step() {

	if (trigger_refresh) {
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				PatternHelpers.hue[j][i] = PatternHelpers.hue_next[j][i];
				PatternHelpers.saturation[j][i] = PatternHelpers.saturation_next[j][i];
				PatternHelpers.value[j][i] = PatternHelpers.value_next[j][i];
			}
		}
		trigger_refresh = false;
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 4; j++) {
			Lumen.leds->set_pixel_rgb(j, i,PatternHelpers.hue[j][i], PatternHelpers.saturation[j][i], PatternHelpers.value[j][i]);
		}
	}

}

void CPatternRaw::set_bank_id(int new_bank_id) {
	if (new_bank_id >= 0 && new_bank_id <= 7) {
		bank_id = new_bank_id;
		bank_id_set = true;
	}
}

void CPatternRaw::set_bank_vals(RGB * color_vals) {
	if (bank_id_set) {
		bank_id_set = false;
		led_id = bank_id * 5;
		PatternHelpers.hue_next[led_id / 10][led_id % 10] = color_vals[0].r;
		PatternHelpers.saturation_next[led_id / 10][led_id % 10] = color_vals[0].g;
		PatternHelpers.value_next[led_id / 10][led_id % 10] = color_vals[0].b;

		PatternHelpers.hue_next[(led_id+1) / 10][(led_id+1) % 10] = color_vals[1].r;
		PatternHelpers.saturation_next[(led_id+1) / 10][(led_id+1) % 10] = color_vals[1].g;
		PatternHelpers.value_next[(led_id+1) / 10][(led_id+1) % 10] = color_vals[1].b;

		PatternHelpers.hue_next[(led_id+2) / 10][(led_id+2) % 10] = color_vals[2].r;
		PatternHelpers.saturation_next[(led_id+2) / 10][(led_id+2) % 10] = color_vals[2].g;
		PatternHelpers.value_next[(led_id+2) / 10][(led_id+2) % 10] = color_vals[2].b;

		PatternHelpers.hue_next[(led_id+3) / 10][(led_id+3) % 10] = color_vals[3].r;
		PatternHelpers.saturation_next[(led_id+3) / 10][(led_id+3) % 10] = color_vals[3].g;
		PatternHelpers.value_next[(led_id+3)/ 10][(led_id+3) % 10] = color_vals[3].b;

		PatternHelpers.hue_next[(led_id+4) / 10][(led_id+4) % 10] = color_vals[4].r;
		PatternHelpers.saturation_next[(led_id+4) / 10][(led_id+4) % 10] = color_vals[4].g;
		PatternHelpers.value_next[(led_id+4) / 10][(led_id+4) % 10] = color_vals[4].b;

	}
}

void CPatternRaw::set_led_id(int new_led_id) {
	if (new_led_id >= 0 && new_led_id <= 39) {
		led_id = new_led_id;
		led_id_set = true;
	}
}

void CPatternRaw::set_led_vals(RGB * color_vals) {
	if (led_id_set) {
		led_id_set = false;
		PatternHelpers.hue_next[led_id / 10][led_id % 10] = color_vals->r;
		PatternHelpers.saturation_next[led_id / 10][led_id % 10] = color_vals->g;
		PatternHelpers.value_next[led_id / 10][led_id % 10] = color_vals->b;
	}
}

void CPatternRaw::set_fill(RGB * color_vals) {
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 4; j++) {
			PatternHelpers.hue[j][i] = color_vals->r;
			PatternHelpers.saturation[j][i] = color_vals->g;
			PatternHelpers.value[j][i] = color_vals->b;
		}
	}
}
void CPatternRaw::set_clear() {
	for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 4; j++) {
				PatternHelpers.hue[j][i] = 0;
				PatternHelpers.saturation[j][i] = 0;
				PatternHelpers.value[j][i] = 0;
			}
		}
}
void CPatternRaw::set_refresh() {
	trigger_refresh = true;
}

uint32_t CPatternRaw::get_pattern_id() {
	return 0x00000013;
}

void CPatternRaw::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternRawConfigBankID: this->set_bank_id((int)data); break;
		case kPatternRawConfigBankVals: this->set_bank_vals((RGB*)data); break;
		case kPatternRawConfigLEDID: this->set_led_id((int)data); break;
		case kPatternRawConfigLEDVals: this->set_led_vals((RGB*)data); break;
		case kPatternRawConfigFill: this->set_fill((RGB*)data); break;
		case kPatternRawConfigClear: this->set_clear(); break;
		case kPatternRawConfigRefresh: this->set_refresh(); break;
		}
}


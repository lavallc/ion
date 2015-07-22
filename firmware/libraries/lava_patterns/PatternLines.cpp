



/*
 * PatternLines.cpp
 *
 *  Created on: Feb 3, 2014
 *      Author: Salemancer
 */
#include "PatternHelpers.h"
#include "PatternLines.h"
#include "sun_board.h"
#include <cmath>


CPatternLines PatternLines;

void CPatternLines::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	for (int i = 0; i < kSunBoardNumberOfLedsPerRay; i++) {
		PatternHelpers.value_next[0][i] = (1 +i) * 25;
	}
	PatternHelpers.progress = 255;
	changes_ready = 0;
	if (configs == NULL) {
		PatternHelpers.hue[0][0] = 240;
		PatternHelpers.saturation[0][0] = 255;
		PatternHelpers.speed = 50;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.hue[0][0] = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		PatternHelpers.speed = configs->config_data[2].data;

	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternLines::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternLinesConfigSetHue;
	configs->config_data[0].data = PatternHelpers.hue[0][0];
	configs->config_data[1].config_id = kPatternLinesConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternLinesConfigSetSpeed;
	configs->config_data[2].data = PatternHelpers.speed;
}


void CPatternLines::step() {

	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] = PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] = PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}

	if (PatternHelpers.progress >= 255) {
		PatternHelpers.progress = 0;
		for (int i = 0; i < kSunBoardNumberOfLedsPerRay; i ++) {
			PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
			if (PatternHelpers.value_next[0][i] == 25) {
				PatternHelpers.value_next[0][i] = 250;
			}
			else {
				PatternHelpers.value_next[0][i] -= 25;
			}

		}
	}
	uint16_t value;
	for (int i = 0; i < kSunBoardNumberOfLedsPerRay; i++) {
		value = PatternHelpers.lerp(PatternHelpers.value[0][i], PatternHelpers.value_next[0][i], PatternHelpers.progress);
		Lumen.leds->set_pixel_linear_hsv(0, i % 10, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], value);
		Lumen.leds->set_pixel_linear_hsv(2, i % 10, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], value);
		Lumen.leds->set_pixel_linear_hsv(1, 9 - (i % 10), PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], value);
		Lumen.leds->set_pixel_linear_hsv(3, 9 - (i % 10), PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], value);
	}
	PatternHelpers.progress += PatternHelpers.speed;
}

uint32_t CPatternLines::get_pattern_id() {
	return 0x0000000D;
}

void CPatternLines::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternLines::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternLines::set_pattern_speed(int new_speed) {
	PatternHelpers.speed = new_speed;
}

void CPatternLines::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternLinesConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternLinesConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternLinesConfigSetSpeed: this->set_pattern_speed((int)data); break;
	}

}

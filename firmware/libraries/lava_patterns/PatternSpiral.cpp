


/*
 * PatternSpiral.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: Salemancer
 *      Pattern Creator: Salemancer
 */

#include "PatternSpiral.h"
#include "PatternHelpers.h"

CPatternSpiral PatternSpiral;

void CPatternSpiral::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	for (int i = 0; i < kSunBoardNumberOfLedsPerRay; i++) {
		pattern_next[i] = 0;
	}
	for (int i = 9; i >= 0; i-=4) {
		pattern_next[i] = 255;
		pattern_next[i-1] = 255;
	}
	PatternHelpers.progress = 255;
	index = 8;
	changes_ready = 0;
	if (configs == NULL) {
		PatternHelpers.hue[0][0] = 0;
		PatternHelpers.saturation[0][0] = 255;
		PatternHelpers.speed = 26;
		value = 190;

	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.hue[0][0] = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		PatternHelpers.speed = configs->config_data[2].data;
		value = configs->config_data[3].data;


	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternSpiral::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternSpiralConfigSetHue;
	configs->config_data[0].data = PatternHelpers.hue[0][0];
	configs->config_data[1].config_id = kPatternSpiralConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternSpiralConfigSetSpeed;
	configs->config_data[2].data = PatternHelpers.speed;
	configs->config_data[3].config_id = kPatternSpiralConfigSetBrightness;
	configs->config_data[3].data = value;
}

void CPatternSpiral::step() {
	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}

	if(PatternHelpers.progress >= 255) {
		//reset moving index and tween PatternHelpers.progress
		PatternHelpers.progress = 0;
		index--;
		if(index == 5)index = 9;

		//lets copy pattern_next into pattern
		for(int i=0; i<kSunBoardNumberOfLedsPerRay; i++) {
			pattern[i] = pattern_next[i];
		}

		//then calculate pattern next
		for(int i=0; i<kSunBoardNumberOfLedsPerRay - 1; i++) {
			pattern_next[i] = pattern_next[i+1];
		}
		if (index == 9 || index == 8)
			pattern_next[9] = PatternHelpers.saturation[0][0];
		else
			pattern_next[9] = 0;
	}
	int s;
	int v;


	//lerp from pattern to pattern_next
	for(int i = 0; i<kSunBoardNumberOfLedsPerRay; i++) {
		//Brightness lerping for red lines.
	/*	if (pattern_next[i] > pattern[i])
			v = PatternHelpers.lerp(50, value, PatternHelpers.progress);
		else if (pattern_next[i] < pattern[i])
			v = PatternHelpers.lerp(value, 50, PatternHelpers.progress);
		else if (pattern_next[i] == 255)
			v = value;
		else
			v = 50;
		*/
		//Lerp hue from bottom up, diagonally.
		s = PatternHelpers.lerp(pattern[i], pattern_next[i], PatternHelpers.progress);
		Lumen.leds->set_pixel_hsv(0,i, PatternHelpers.hue[0][0], s, value);
		Lumen.leds->set_pixel_hsv(1,i-3, PatternHelpers.hue[0][0], s, value);
		Lumen.leds->set_pixel_hsv(2,i-2, PatternHelpers.hue[0][0], s, value);
		Lumen.leds->set_pixel_hsv(3,i-1, PatternHelpers.hue[0][0], s, value);
	}
	for(int i = 6; i<kSunBoardNumberOfLedsPerRay; i++) {
		//Brightness lerping for red lines.
	/*	if (pattern_next[i] > pattern[i])
			v = PatternHelpers.lerp(50, value, PatternHelpers.progress);
		else if (pattern_next[i] < pattern[i])
			v = PatternHelpers.lerp(value, 50, PatternHelpers.progress);
		else if (pattern_next[i] == 255)
			v = value;
		else
			v = 50;
		*/
		//Lerp hue from mid down diagonally.
		s = PatternHelpers.lerp(pattern[i], pattern_next[i], PatternHelpers.progress);
		Lumen.leds->set_pixel_hsv(1,i+1, PatternHelpers.hue[0][0], s, value);
		Lumen.leds->set_pixel_hsv(2,i+2, PatternHelpers.hue[0][0], s, value);
		Lumen.leds->set_pixel_hsv(3,i+3, PatternHelpers.hue[0][0], s, value);
	}
	PatternHelpers.progress += PatternHelpers.speed;
}

void CPatternSpiral::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternSpiral::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}


void CPatternSpiral::set_pattern_brightness(int new_brightness) {
	value = new_brightness;
}

void CPatternSpiral::set_pattern_speed(int new_speed) {
	PatternHelpers.speed = new_speed;
}

uint32_t CPatternSpiral::get_pattern_id() {
	return 0x00000006;
}

void CPatternSpiral::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternSpiralConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternSpiralConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternSpiralConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternSpiralConfigSetBrightness: this->set_pattern_brightness((int)data); break;
		}
}


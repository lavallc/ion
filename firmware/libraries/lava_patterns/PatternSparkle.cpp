/*
 * PatternSparkle.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: salem
 */

#include "PatternSparkle.h"
#include "PatternHelpers.h"
#include "sun_board.h"
#include <cmath>
CPatternSparkle PatternSparkle;

void CPatternSparkle::start(LumenMoodConfig * configs, bool restore_to_defaults) {

	position = 0;
	for (int i = 0; i < 20; i++) {
		ledvals[i] = 0;
		rayvals[i] = 0;
		progress[i] = 255;
	}
	delay = 0;
	changes_ready = 0;
	power = 15;
	if (configs == NULL) {
		brightness = 200;
		PatternHelpers.speed = 1;
		PatternHelpers.hue[0][0] = 200;
		PatternHelpers.saturation[0][0] = 255;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.hue[0][0] = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		set_pattern_speed(configs->config_data[2].data);
		set_pattern_brightness(configs->config_data[3].data);
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}

void CPatternSparkle::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternSparkleConfigSetHue;
	configs->config_data[0].data = PatternHelpers.hue[0][0];
	configs->config_data[1].config_id = kPatternSparkleConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternSparkleConfigSetSpeed;
	configs->config_data[2].data = PatternHelpers.speed * 10;
	configs->config_data[3].config_id = kPatternSparkleConfigSetBrightness;
	int temp_brightness = brightness;
	temp_brightness = ((temp_brightness - 100) * 5) / 3;
	configs->config_data[3].data = temp_brightness;
}
void CPatternSparkle::step() {

	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}
	delay++;
	//Every 8 steps, we see if the last element has been completed its tween.
	//If so, we create a new element to tween.
	if (rand() % power == 0) {
		delay = 0;
		int val = (position + 1) % 20;
		if (progress[val] == 255) {
			position = val;
			ledvals[position] = rand() % 10;
			rayvals[position] = rand() % 4;
			progress[position] = 0;
		}
	}
	//lerp from pattern to pattern_next
	Lumen.leds->set_solid_linear_hsv(PatternHelpers.hue[0][0], 255, 102);
	for (int i = 0; i < 20; i++) {
		progress[i] = PatternHelpers.value_cap(progress[i] + PatternHelpers.speed, 255, 0); //saturation gets incremented by PatternHelpers.speed each time.
		int s = PatternHelpers.lerp(PatternHelpers.saturation[0][0] * 0.5, PatternHelpers.saturation[0][0], progress[i]);			// saturation lerps from 50% to 100%
		int b = PatternHelpers.lerp(brightness, 100, progress[i]);			// brightness lerps from 100(full) to 40(background)
		int h = PatternHelpers.lerp(PatternHelpers.hue[0][0]+30, PatternHelpers.hue[0][0], progress[i]) % 360; 	// hue lerps from hue+30 to background hue.
		Lumen.leds->set_pixel_linear_hsv(rayvals[i], ledvals[i], h, s, b);
	}
}

void CPatternSparkle::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}
void CPatternSparkle::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternSparkle::set_pattern_speed(int new_speed) {
	PatternHelpers.speed = new_speed / 10;
}

void CPatternSparkle::set_pattern_brightness(int new_brightness) {
	new_brightness = (new_brightness * 3) / 5;
	brightness = new_brightness + 100;
}

uint32_t CPatternSparkle::get_pattern_id() {
	return 0x00000000E;
}

void CPatternSparkle::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternSparkleConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternSparkleConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternSparkleConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternSparkleConfigSetBrightness: this->set_pattern_brightness((int)data); break;
	}
}

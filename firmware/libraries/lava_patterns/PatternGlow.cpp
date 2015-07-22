/*
 * PatternGlow.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: salem
 */

#include "PatternGlow.h"
#include "sun_board.h"
#include <cmath>

CPatternGlow PatternGlow;

const double PI  =3.141592653589793238463;

void CPatternGlow::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	angle = 0;

	if (configs == NULL) {
		speed = 10;
		value = 255;
		swing = 127;
		hue = 120;
		saturation = 100;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		hue = configs->config_data[0].data;
		set_pattern_speed(configs->config_data[1].data);
		value = configs->config_data[2].data;
		//swing = configs->config_data[2].data;
		saturation = configs->config_data[3].data;
		swing = value / 2;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternGlow::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].data = hue;
	configs->config_data[0].config_id = kPatternGlowConfigSetHue;
	configs->config_data[1].data = speed * 4;
	configs->config_data[1].config_id = kPatternGlowConfigSetSpeed;
	configs->config_data[2].data = value;
	configs->config_data[2].config_id = kPatternGlowConfigSetBrightness;
	configs->config_data[3].data = saturation;
	configs->config_data[3].config_id = kPatternGlowConfigSetSaturation;
}
double CPatternGlow::radians (double d) {
	return d * PI / 180;
}


void CPatternGlow::step() {

	if(angle > 360) angle = 0;

	brightness = sin(radians(angle))*((double)swing / 255.0) + 0.5;
	if (brightness > 1) brightness = 1;
	if (brightness < 0) brightness = 0;

	Lumen.leds->set_solid_hsv(hue,saturation,value*brightness);
	angle += speed;
}

uint32_t CPatternGlow::get_pattern_id() {
	return 0x00000016;
}

void CPatternGlow::set_pattern_speed(int new_speed) {
	speed= new_speed / 4;
}

void CPatternGlow::set_pattern_brightness(int new_brightness) {
	this->value= new_brightness;
}

void CPatternGlow::set_pattern_brightness_swing(int new_brightness_swing) {
	this->swing= new_brightness_swing;
}

void CPatternGlow::set_pattern_hue(int new_hue) {
	this->hue= new_hue;
}

void CPatternGlow::set_pattern_saturation(int new_saturation) {
	this->saturation= new_saturation;
}


void CPatternGlow::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternGlowConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternGlowConfigSetBrightness: this->set_pattern_brightness((int)data); break;
		case kPatternGlowConfigSetBrightnessSwing: this->set_pattern_brightness_swing((int)data); break;
		case kPatternGlowConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternGlowConfigSetSaturation: this->set_pattern_hue((int)data); break;
		break;
	}
}

/*
 * PatternSpin.cpp
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#include "PatternSpin.h"
#include "sun_board.h"
#include <cmath>

CPatternSpin PatternSpin;

const double PI  =3.141592653589793238463;

void CPatternSpin::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	brightness = 1.0f;
	angle = 0;


	if (configs == NULL) {
		speed = 3;
		value = 255;
		hue = 0;
		saturation = 255;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		hue = configs->config_data[0].data;
		speed = configs->config_data[1].data / 6;
		value = configs->config_data[2].data;
		saturation = configs->config_data[3].data;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternSpin::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].data = hue;
	configs->config_data[0].config_id = kPatternSpinConfigSetHue;
	configs->config_data[1].data = speed * 6;
	configs->config_data[1].config_id = kPatternSpinConfigSetSpeed;
	configs->config_data[2].data = value;
	configs->config_data[2].config_id = kPatternSpinConfigSetBrightness;
	configs->config_data[3].data = saturation;
	configs->config_data[3].config_id = kPatternSpinConfigSetSaturation;

}

double CPatternSpin::radians (double d) {
	return d * PI / 180;
}

double tooth(double d) {
	//if (d > 0 && d < 60)
	//	return d / 146 + 0.25;
	//else if (d >= 60 && d <= 90)
	if (d > 0 &&  d <= 90)
		return (d/90.0);
	else if (d > 90 && d <= 180)
		return (1- (d-90.0)/90.0);
	else
		return 0;
}

void CPatternSpin::step() {

	if(angle > 360) angle = 0;

	for (int i = 0; i < kSunBoardNumberOfRays; i++) {
		set_angle = angle+(i*90);
		set_angle = set_angle % 360;
		brightness = tooth(set_angle);
		//brightness = sin(radians(set_angle));
		if (brightness > 1) brightness = 1;
		if (brightness < 0) brightness = 0;

		Lumen.leds->set_ray_hsv(i,hue,saturation,value*brightness);
	}
	angle += speed;
}

uint32_t CPatternSpin::get_pattern_id() {
	return 0x00000009;
}

void CPatternSpin::set_pattern_speed(int new_speed) {
	speed = new_speed / 6;
}

void CPatternSpin::set_pattern_brightness(int new_brightness) {
	this->value= new_brightness;
}

void CPatternSpin::set_pattern_hue(int new_hue) {
	this->hue= new_hue;
}

void CPatternSpin::set_pattern_saturation(int new_saturation) {
	this->saturation= new_saturation;
}


void CPatternSpin::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternSpinConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternSpinConfigSetBrightness: this->set_pattern_brightness((int)data); break;
		case kPatternSpinConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternSpinConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		break;
	}
}


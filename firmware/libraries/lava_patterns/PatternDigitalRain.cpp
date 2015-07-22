/*
 * PatternDigitalRain.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: salem
 */

#include "PatternDigitalRain.h"
#include "PatternHelpers.h"
#include "Serial.h"
CPatternDigitalRain PatternDigitalRain;

void CPatternDigitalRain::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	changes_ready = 0;
	if (configs == NULL) {

		PatternHelpers.speed = 6;
		PatternHelpers.hue[0][0] = 120;
		PatternHelpers.saturation[0][0] = 255;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.hue[0][0] = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		set_pattern_speed(configs->config_data[2].data);

	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
	delay = 0;
}

void CPatternDigitalRain::get_pattern_config(LumenMoodConfig * configs) {
	configs->config_data[0].config_id = kPatternDigitalRainConfigSetHue;
	configs->config_data[0].data = PatternHelpers.hue[0][0];
	configs->config_data[1].config_id = kPatternDigitalRainConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternDigitalRainConfigSetSpeed;
	if (PatternHelpers.speed == 1)
		configs->config_data[2].data = 9;
	else if (PatternHelpers.speed == 2)
		configs->config_data[2].data = 24;
	else if (PatternHelpers.speed == 3)
		configs->config_data[2].data = 49;
	else if (PatternHelpers.speed == 4)
		configs->config_data[2].data = 74;
	else if (PatternHelpers.speed == 5)
		configs->config_data[2].data = 99;
	else if (PatternHelpers.speed == 25)
		configs->config_data[2].data = 255;
	else
		configs->config_data[2].data = (PatternHelpers.speed * 17);

	configs->config_data[2].data = 255 - configs->config_data[2].data;
}

void CPatternDigitalRain::step() {

	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}
	if ((delay < 1) && (PatternHelpers.speed != 25)) {
		delay = PatternHelpers.speed;
		if (rand() % 5 == 0) {
			ray1[0] = 25;
			ray1[1] = 125;
			ray1[2] = 125;
			ray1[3] = 190;
			ray1[4] = 255;
		}
		if (rand() % 5 == 0) {
			ray2[0] = 25;
			ray2[1] = 125;
			ray2[2] = 125;
			ray2[3] = 190;
			ray2[4] = 255;
		}
		if (rand() % 5 == 0) {
			ray3[0] = 25;
			ray3[1] = 125;
			ray3[2] = 125;
			ray3[3] = 190;
			ray3[4] = 255;
		}
		if (rand() % 5 == 0) {
			ray4[0] = 25;
			ray4[1] = 125;
			ray4[2] = 125;
			ray4[3] = 190;
			ray4[4] = 255;
		}
		//Clear past displayed lights
		Lumen.leds->set_solid_rgb(0, 0, 0);
		//Show the current rain
		for (int j = 13; j >= 4; j--) {
			Lumen.leds->set_pixel_linear_hsv(0, j - 4, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], ray1[j]);
			Lumen.leds->set_pixel_linear_hsv(1, j - 4, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], ray2[j]);
			Lumen.leds->set_pixel_linear_hsv(2, j - 4, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], ray3[j]);
			Lumen.leds->set_pixel_linear_hsv(3, j - 4, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], ray4[j]);
		}

		//Shift rain down for the next step
		for (int j = 13; j > 0; j--) {
			ray1[j] = ray1[j - 1];
			ray2[j] = ray2[j - 1];
			ray3[j] = ray3[j - 1];
			ray4[j] = ray4[j - 1];
		}
		//Clear top because of the shift down.
		ray1[0] = 0;
		ray2[0] = 0;
		ray3[0] = 0;
		ray4[0] = 0;
	}
	delay--;
	if (delay < 0) delay = 0;
}

uint32_t CPatternDigitalRain::get_pattern_id() {
	return 0x00000003;
}

void CPatternDigitalRain::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternDigitalRain::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternDigitalRain::set_pattern_speed(int new_speed) {
	new_speed = 255 - new_speed;
	if (new_speed < 10)
		PatternHelpers.speed = 1;
	else if (new_speed < 25)
		PatternHelpers.speed = 2;
	else if (new_speed < 50)
		PatternHelpers.speed = 3;
	else if (new_speed < 75)
		PatternHelpers.speed = 4;
	else if (new_speed < 100)
		PatternHelpers.speed = 5;
	else if (new_speed == 255)
		PatternHelpers.speed = 25;
	else
		PatternHelpers.speed = (new_speed / 17);
}

void CPatternDigitalRain::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternDigitalRainConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternDigitalRainConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternDigitalRainConfigSetSpeed: this->set_pattern_speed((int)data); break;
	}
}

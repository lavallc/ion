/*
 * PatternBinaryClock.cpp
 *
 *  Created on: Feb 13, 2014
 *      Author: barch
 */

#include "PatternHelpers.h"
#include "PatternBinaryClock.h"
#include "sun_board.h"
#include "Lumen.h"
#include <cmath>


CPatternBinaryClock PatternBinaryClock;

void CPatternBinaryClock::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	Lumen.leds->set_solid_rgb(0,0,0);
}
void CPatternBinaryClock::get_pattern_config(LumenMoodConfig * configs) {

}

void CPatternBinaryClock::step() {
	// hour
	display_binary_strip(0, LumenClock.get_hours_24(), hour_hue);

	// minute
	display_binary_strip(1, LumenClock.get_minutes(), minute_hue);

	// second
	display_binary_strip(2, LumenClock.get_seconds(), second_hue);
}

uint32_t CPatternBinaryClock::get_pattern_id() {
	return 0x0000000F;
}

void CPatternBinaryClock::display_binary_strip(unsigned char ray, unsigned char value, int hue) {
	if (value >= 32) {
		Lumen.leds->set_pixel_hsv(ray, 4, hue, 255, 255);
		value -= 32;
	} else {
		Lumen.leds->set_pixel_hsv(ray, 4, hue, 255, 255);
	}
	if (value >= 16) {
		Lumen.leds->set_pixel_hsv(ray, 5, hue, 255, 255);
		value -= 16;
	} else {
		Lumen.leds->set_pixel_hsv(ray, 5, hue, 255, 0);
	}
	if (value >= 8) {
		Lumen.leds->set_pixel_hsv(ray, 6, hue, 255, 255);
		value -= 8;
	} else {
		Lumen.leds->set_pixel_hsv(ray, 6, hue, 255, 0);
	}
	if (value >= 4) {
		Lumen.leds->set_pixel_hsv(ray, 7, hue, 255, 255);
		value -= 4;
	} else {
		Lumen.leds->set_pixel_hsv(ray, 7, hue, 255, 0);
	}
	if (value >= 2) {
		Lumen.leds->set_pixel_hsv(ray, 8, hue, 255, 255);
		value -= 2;
	} else {
		Lumen.leds->set_pixel_hsv(ray, 8, hue, 255, 0);
	}
	if (value >= 1) {
		Lumen.leds->set_pixel_hsv(ray, 9, hue, 255, 255);
	} else {
		Lumen.leds->set_pixel_hsv(ray, 9, hue, 255, 0);
	}
}

void CPatternBinaryClock::set_pattern_hue(uint16_t hr_hue, uint16_t min_hue, uint16_t sec_hue) {
	hour_hue = hr_hue;
	minute_hue = min_hue;
	second_hue = sec_hue;
}

void CPatternBinaryClock::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternBinaryClockConfigSetHue: this->set_pattern_hue((unsigned char)data, (unsigned char)(data >> 8), (unsigned char)(data >> 16)); break;
	}
}

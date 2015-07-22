



/*
 * PatternHourglas.cpp
 *
 *  Created on: Feb 6, 2014
 *      Author: Salemancer
 */
#include "PatternHelpers.h"
#include "PatternHourglass.h"
#include "sun_board.h"
#include <cmath>


CPatternHourglass PatternHourglass;

void CPatternHourglass::start(LumenMoodConfig * configs, bool restore_to_defaults) {

	is_running = false;
	changes_ready = 0;
	if (configs == NULL) {
		total_seconds = 60;
		last_second = total_seconds;
		PatternHelpers.hue[0][0] = 44;
		PatternHelpers.saturation[0][0] = 255;
		//Get Timer information and add 60seconds_left to the clock.
		timer_start = LumenClock.get_uptime();
		timer_end = timer_start + total_seconds;
		tick_counter = 0;
		Lumen.leds->set_solid_rgb(0,0,0);
		seconds_left = timer_end - LumenClock.get_uptime();
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		total_seconds = configs->config_data[0].data;
		last_second = total_seconds;
		PatternHelpers.hue[0][0] = configs->config_data[1].data;
		PatternHelpers.saturation[0][0] = configs->config_data[2].data;
		timer_start = LumenClock.get_uptime();
		timer_end = timer_start + total_seconds;
		tick_counter = 0;
		Lumen.leds->set_solid_rgb(0,0,0);
		seconds_left = timer_end - LumenClock.get_uptime();
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}

}
void CPatternHourglass::get_pattern_config(LumenMoodConfig * configs) {
	unsigned int time = timer_end - timer_start;
	configs->config_data[0].config_id = kPatternHourglassConfigSetTime;
	configs->config_data[0].data = time;
	configs->config_data[1].config_id = kPatternHourglassConfigSetHue;
	configs->config_data[1].data = PatternHelpers.hue[0][0];
	configs->config_data[2].config_id = kPatternHourglassConfigSetSaturation;
	configs->config_data[2].data = PatternHelpers.saturation[0][0];
}


void CPatternHourglass::step() {

	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}
	if (is_running) {
		seconds_left = timer_end - LumenClock.get_uptime();
	}
	else {

	}

	// every time a second ticks by, reset the tick counter
	if (seconds_left != last_second) {
		tick_counter = 0;
	}


	//While we haven't ran out of time, keep updating.
	if (seconds_left >= 0) {
		if (is_running)
			tick_counter++;

		amount_top = (seconds_left * 33.0 + (34-tick_counter)) * 1275 / (total_seconds * 33.0);	//Total amount of sand(255%) scaled to 1275.

		amount_bottom = 1275 - amount_top;
		/////////////// TOP HALF
		Lumen.leds->set_level_linear_hsv(0,PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], PatternHelpers.value_cap((amount_top - 637) * 2/5, 255, 0)); //Top LED is 50% sand
		Lumen.leds->set_level_linear_hsv(1,PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0] * 0.9, PatternHelpers.value_cap((amount_top - 191) * 2.2/5,191, 0)); //Mid LED is 35% sand
		Lumen.leds->set_level_linear_hsv(2,PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0] * 0.78, PatternHelpers.value_cap(amount_top, 153, 40));				  //Bot LED is 15% sand
		////////////////

		//Drop the sand by moving it down every step.
		if (is_running)
			grain++;
		if (grain > 6)
			grain = 0;
		Lumen.leds->set_level_rgb(3,0,0,0);
		Lumen.leds->set_level_rgb(4,0,0,0);
		Lumen.leds->set_level_rgb(5,0,0,0);
		Lumen.leds->set_level_rgb(6,0,0,0);
		Lumen.leds->set_level_rgb(7,0,0,0);
		Lumen.leds->set_level_rgb(8,0,0,0);
		if (seconds_left == 0 && tick_counter > 20 && grain >3) {
			is_running = false;
			Lumen.leds->set_level_hsv(2, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], 0);
		}
		else {
			Lumen.leds->set_level_linear_hsv(grain + 3, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0] * 0.78 , 100);
		}

		//////////////// BOTTOM HALF
		if (seconds_left <= 0.2 * total_seconds)	//Only overwrite the dropping sand if we need to fill this LED
			Lumen.leds->set_level_linear_hsv(7, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0] * 0.78, PatternHelpers.value_cap((amount_bottom - 1083) ,153,0));//Top LED is 15% sand
		if (seconds_left <= 0.5 * total_seconds)	//Only overwrite the dropping sand if we need to fill this LED
			Lumen.leds->set_level_linear_hsv(8, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0] * 0.9, PatternHelpers.value_cap((amount_bottom - 637) * 2.2/5,191, 0));//Mid LED is 35% sand
		Lumen.leds->set_level_linear_hsv(9, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], PatternHelpers.value_cap(amount_bottom * 2/5, 255, 0));   //Bot LED is 50% sand
		//////////////////
	}

	last_second = seconds_left;
}

uint32_t CPatternHourglass::get_pattern_id() {
	return 0x00000010;
}

void CPatternHourglass::set_pattern_time(int time) {
	// ignore zero times
	if (time == 0)
		return;

	timer_start = LumenClock.get_uptime();
	timer_end = timer_start + time;
	total_seconds = time;
	tick_counter = 0;
	is_running = true;
	last_second = total_seconds;
	Lumen.leds->set_solid_rgb(0,0,0);
	seconds_left = timer_end - LumenClock.get_uptime();
}

void CPatternHourglass::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}
void CPatternHourglass::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}


void CPatternHourglass::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternHourglassConfigSetTime: this->set_pattern_time((int)data); break;
		case kPatternHourglassConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternHourglassConfigSetSaturation: this->set_pattern_saturation((int)data); break;
	}
}

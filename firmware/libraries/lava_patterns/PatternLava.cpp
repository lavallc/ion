/*
 * PatternLava.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: salem
 */

#include "PatternLava.h"
#include "PatternHelpers.h"
#include "Serial.h"
#define FULL_BRIGHTNESS 150
CPatternLava PatternLava;

void CPatternLava::start(LumenMoodConfig * configs, bool restore_to_defaults){

	changes_ready = 0;
	if (configs == NULL) {
		PatternHelpers.erase_all();
		PatternHelpers.progress = 255;
		PatternHelpers.speed = 2;
		PatternHelpers.hue[0][0] = 0;
		PatternHelpers.saturation[0][0] = 255;
		position = 0;
		led_num[0] = 9;
		led_num[1] = 9;
		led_num[2] = 9;
		led_num[3] = 9;
		ray_num[0] = rand() % 4;
		direction[0] = -1;
		direction[1] = 0;
		direction[2] = 0;
		direction[3] = 0;

	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.erase_all();
		PatternHelpers.progress = 255;
		PatternHelpers.speed = configs->config_data[0].data / 5;
		PatternHelpers.hue[0][0] = configs->config_data[1].data;
		PatternHelpers.saturation[0][0] = configs->config_data[2].data;
		position = 0;
		led_num[0] = 9;
		led_num[1] = 9;
		led_num[2] = 9;
		led_num[3] = 9;
		ray_num[0] = rand() % 4;
		direction[0] = -1;
		direction[1] = 0;
		direction[2] = 0;
		direction[3] = 0;

	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}

	PatternHelpers.value_next[0][9] = FULL_BRIGHTNESS;
	PatternHelpers.value_next[1][9] = FULL_BRIGHTNESS;
	PatternHelpers.value_next[2][9] = FULL_BRIGHTNESS;
	PatternHelpers.value_next[3][9] = FULL_BRIGHTNESS;
	PatternHelpers.value[0][9] = FULL_BRIGHTNESS;
	PatternHelpers.value[1][9] = FULL_BRIGHTNESS;
	PatternHelpers.value[2][9] = FULL_BRIGHTNESS;
	PatternHelpers.value[3][9] = FULL_BRIGHTNESS;
}
void CPatternLava::get_pattern_config(LumenMoodConfig * configs) {
	configs->config_data[0].config_id = kPatternLavaConfigSetSpeed;
	configs->config_data[0].data = PatternHelpers.speed * 5;
	configs->config_data[1].config_id = kPatternLavaConfigSetHue;
	configs->config_data[1].data = PatternHelpers.hue[0][0];
	configs->config_data[2].config_id = kPatternLavaConfigSetSaturation;
	configs->config_data[2].data = PatternHelpers.saturation[0][0];
}
void CPatternLava::step() {

	if (changes_ready == 3) {
			PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
			PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
			changes_ready = 0;
		}

	if(PatternHelpers.progress >= 255) {
		PatternHelpers.progress = 0;
		//lerp from pattern to PatternHelpers.value_next
		for(int i = 0; i<4; i++) {
			for(int j=0; j<10;j++) {
				PatternHelpers.value[i][j] = PatternHelpers.value_next[i][j];
			}
		}
		//Select a new ray to setup a new goo.
		if (rand() % 25 == 0) {
			int8_t val = (position + 1) % 4;
			if (direction[val] == 0) {
				position = val;
				direction[position] = -1;
				led_num[position] = 9;
				ray_num[position] = rand() % 4;
			}
		}
		//Do all the movement.
		for (int i = 0; i < 4; i++){
			int RIGHT = (ray_num[i]+1)%4;
			int MID = ray_num[i];
			//Turn goo to head back down.
			if (led_num[i] == 0) {
				direction[i] = 1;
				led_num[i] = 1;
			}
			//Stop goo when back at bottom
			if (led_num[i] == 9 && direction[i] == 1) {
				PatternHelpers.value_next[MID][led_num[i]-direction[i]] = 0;
				PatternHelpers.value_next[RIGHT][led_num[i]-direction[i]] = 0;
				direction[i] = 0;
			}
			if (direction[i] != 0) {	//equivalent of (if goo is moving)
				bool trigger_blob_rise = false;

				//Pull from mid row and move to top row
				int shift_mid_amount = rand() % PatternHelpers.value_next[MID][led_num[i]];
				int shift_right_amount = rand() % PatternHelpers.value_next[RIGHT][led_num[i]];
				if (led_num[i]+direction[i] < 9) {
					//MID check for overflow
					if (shift_mid_amount + PatternHelpers.value_next[MID][led_num[i]+direction[i]] > FULL_BRIGHTNESS) {
						shift_mid_amount -= (FULL_BRIGHTNESS - PatternHelpers.value_next[MID][led_num[i]+direction[i]]);
						PatternHelpers.value_next[MID][led_num[i]+direction[i]] = FULL_BRIGHTNESS;
					}
					else {
						PatternHelpers.value_next[MID][led_num[i]+direction[i]] += shift_mid_amount;
					}
					//RIGHT check for overflow
					if (shift_right_amount + PatternHelpers.value_next[RIGHT][led_num[i]+direction[i]] > FULL_BRIGHTNESS) {
						shift_right_amount -= (FULL_BRIGHTNESS - PatternHelpers.value_next[RIGHT][led_num[i]+direction[i]]);
						PatternHelpers.value_next[RIGHT][led_num[i]+direction[i]] = FULL_BRIGHTNESS;
					}
					else {
						PatternHelpers.value_next[RIGHT][led_num[i]+direction[i]] += shift_right_amount;
					}
				}
				if (PatternHelpers.value_next[MID][led_num[i]+direction[i]] >= FULL_BRIGHTNESS &&
						PatternHelpers.value_next[RIGHT][led_num[i]+direction[i]] >= FULL_BRIGHTNESS ){
					trigger_blob_rise = true;
				}
				if (led_num[i] < 9 && led_num[i] > 0) {
					//adjust mid row values
					PatternHelpers.value_next[MID][led_num[i]] -= shift_mid_amount;
					PatternHelpers.value_next[RIGHT][led_num[i]] -= shift_right_amount;

					if (PatternHelpers.value[MID][led_num[i]-direction[i]] > 50 && !trigger_blob_rise)
						shift_mid_amount = rand() % PatternHelpers.value[MID][led_num[i]-direction[i]];
					else
						shift_mid_amount = PatternHelpers.value[MID][led_num[i]-direction[i]];
					if (PatternHelpers.value[RIGHT][led_num[i]-direction[i]] > 50 && !trigger_blob_rise)
						shift_right_amount = rand() % PatternHelpers.value[RIGHT][led_num[i]-direction[i]];
					else
						shift_right_amount = PatternHelpers.value[RIGHT][led_num[i]-direction[i]];

					if (shift_mid_amount + PatternHelpers.value_next[MID][led_num[i]] >= FULL_BRIGHTNESS) {
						shift_mid_amount -= (FULL_BRIGHTNESS - PatternHelpers.value_next[MID][led_num[i]]);
						PatternHelpers.value_next[MID][led_num[i]] = FULL_BRIGHTNESS;
					}
					else {
						PatternHelpers.value_next[MID][led_num[i]] +=shift_mid_amount;
					}

					if (shift_right_amount + PatternHelpers.value_next[RIGHT][led_num[i]] >= FULL_BRIGHTNESS) {
						shift_right_amount -= (FULL_BRIGHTNESS - PatternHelpers.value_next[RIGHT][led_num[i]]);
						PatternHelpers.value_next[RIGHT][led_num[i]] = FULL_BRIGHTNESS;
					}
					else {
						PatternHelpers.value_next[RIGHT][led_num[i]] += shift_right_amount;
					}

					if (led_num[i] < 8 || (led_num[i] == 8 && direction[i] == 1)) {
						//adjust bottom row values
						if (trigger_blob_rise) {
							if (PatternHelpers.value_next[MID][led_num[i]] + PatternHelpers.value_next[MID][led_num[i]-direction[i]] <= 255)
								PatternHelpers.value_next[MID][led_num[i]] += PatternHelpers.value_next[MID][led_num[i]-direction[i]];
							else
								PatternHelpers.value_next[MID][led_num[i]] = FULL_BRIGHTNESS;
							if (PatternHelpers.value_next[RIGHT][led_num[i]] + PatternHelpers.value_next[RIGHT][led_num[i]-direction[i]] <= 255)
								PatternHelpers.value_next[RIGHT][led_num[i]] += PatternHelpers.value_next[RIGHT][led_num[i]-direction[i]];
							else
								PatternHelpers.value_next[RIGHT][led_num[i]] = FULL_BRIGHTNESS;
							PatternHelpers.value_next[MID][led_num[i]-direction[i]] = 0;
							PatternHelpers.value_next[RIGHT][led_num[i]-direction[i]] = 0;
						}
						else {
							PatternHelpers.value_next[MID][led_num[i]-direction[i]] -= shift_mid_amount;
							PatternHelpers.value_next[RIGHT][led_num[i]-direction[i]] -= shift_right_amount;
						}

						if ((PatternHelpers.value_next[MID][led_num[i]-direction[i]] == 0 &&
								PatternHelpers.value_next[RIGHT][led_num[i]-direction[i]] == 0)  || trigger_blob_rise ) {
							led_num[i] += direction[i];
						}
					}
					else if (trigger_blob_rise) {
						led_num[i] += direction[i];
					}
				}
				else if (trigger_blob_rise) {

					led_num[i] += direction[i];
				}
			}
		}

		PatternHelpers.value_next[0][9] = FULL_BRIGHTNESS;
		PatternHelpers.value_next[1][9] = FULL_BRIGHTNESS;
		PatternHelpers.value_next[2][9] = FULL_BRIGHTNESS;
		PatternHelpers.value_next[3][9] = FULL_BRIGHTNESS;
	}

	//lerp from pattern to PatternHelpers.value_next
	for(int i = 0; i<4; i++) {
		for(int j=0; j<10;j++) {
			int b = PatternHelpers.lerp(PatternHelpers.value[i][j], PatternHelpers.value_next[i][j], PatternHelpers.progress);
			Lumen.leds->set_pixel_hsv(i,j, PatternHelpers.hue[0][0], PatternHelpers.saturation[0][0], PatternHelpers.value_cap(b, 255,0));
		}
	}
	PatternHelpers.progress+=PatternHelpers.speed;
}

uint32_t CPatternLava::get_pattern_id() {
	return 0xB;
}

void CPatternLava::set_pattern_speed(int new_speed) {
	PatternHelpers.speed = new_speed / 5; // 1-50
}

void CPatternLava::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}
void CPatternLava::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternLava::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternLavaConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternLavaConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternLavaConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		break;
	}
}

/*
 * PatternFlame.cpp
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#include "PatternFlame.h"
#include "sun_board.h"
#include "LumenSettings.h"
#include "PatternHelpers.h"

CPatternFlame PatternFlame;

void CPatternFlame::display() {
	for (int i = 2; i < kSunBoardNumberOfLedsPerRay; i++) {
		Lumen.leds->set_pixel_hsv(0,i,PatternHelpers.value_cap(PatternHelpers.hue[0][i],360,0), PatternHelpers.saturation[0][0], PatternHelpers.value_cap(PatternHelpers.value[0][i],255,0));
		Lumen.leds->set_pixel_hsv(1,i,PatternHelpers.value_cap(PatternHelpers.hue[1][i],360,0), PatternHelpers.saturation[0][0], PatternHelpers.value_cap(PatternHelpers.value[1][i],255,0));
		Lumen.leds->set_pixel_hsv(2,i,PatternHelpers.value_cap(PatternHelpers.hue[2][i],360,0), PatternHelpers.saturation[0][0], PatternHelpers.value_cap(PatternHelpers.value[2][i],255,0));
		Lumen.leds->set_pixel_hsv(3,i,PatternHelpers.value_cap(PatternHelpers.hue[3][i],360,0), PatternHelpers.saturation[0][0], PatternHelpers.value_cap(PatternHelpers.value[3][i],255,0));
	}
}
void CPatternFlame::get_pattern_config(LumenMoodConfig * configs) {
	configs->config_data[0].config_id = kPatternFlameConfigSetHue;
	configs->config_data[0].data = hue;
	configs->config_data[1].config_id = kPatternFlameConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternFlameConfigSetSize;
	configs->config_data[2].data = size;
	configs->config_data[3].config_id = kPatternFlameConfigSetSoundEnabled;
	configs->config_data[3].data = sound_enabled;
}

void CPatternFlame::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	if (configs == NULL) {
		PatternHelpers.erase_value();
		PatternHelpers.erase_hue();
		PatternHelpers.erase_saturation();
		Lumen.leds->set_solid_rgb(0,0,0);
		PatternHelpers.progress = 255;
		PatternHelpers.speed = 50;
		changes_ready = 0;
		hue = 0;
		size = 0;
		sound_enabled = true;
		PatternHelpers.saturation[0][0] = 255;

	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.erase_value();
		PatternHelpers.erase_hue();
		PatternHelpers.erase_saturation();
		Lumen.leds->set_solid_rgb(0,0,0);
		PatternHelpers.progress = 255;
		PatternHelpers.speed = 50;
		changes_ready = 0;
		hue = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		size = configs->config_data[2].data;
		sound_enabled = configs->config_data[3].data;
		int ray_num = rand() % 4;
		int led_num = rand() % 2;
		for (int i = 2; i < kSunBoardNumberOfLedsPerRay; i++) {
			//If the template is shifted down, then the top of the tube should be manually set.
			if ((i - led_num) < 0) {
				PatternHelpers.hue[0][i] = 0;
				PatternHelpers.hue[1][i] = 0;
				PatternHelpers.hue[2][i] = 0;
				PatternHelpers.hue[3][i] = 0;
			}
			else {
				PatternHelpers.hue[0][i] = (flame_hue[ray_num][i - led_num] + hue) % 360;
				PatternHelpers.hue[1][i] = (flame_hue[(ray_num + 1) % 4][i - led_num] + hue) % 360;
				PatternHelpers.hue[2][i] = (flame_hue[(ray_num + 2) % 4][i - led_num] + hue) % 360;
				PatternHelpers.hue[3][i] = (flame_hue[(ray_num + 3) % 4][i - led_num] + hue) % 360;
				PatternHelpers.value[0][i] = flame_value[ray_num][i - led_num];
				PatternHelpers.value[1][i] = flame_value[(ray_num + 1) % 4][i - led_num];
				PatternHelpers.value[2][i] = flame_value[(ray_num + 2) % 4][i - led_num];
				PatternHelpers.value[3][i] = flame_value[(ray_num + 3) % 4][i - led_num];
			}
		}

	}
	else {
		Lumen.leds->set_solid_rgb(0,0,0);
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}

void CPatternFlame::step() {

	if (changes_ready == 3) {
		hue = temp_hue;
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}

	if(PatternHelpers.progress >= 125) {
		//get random position
		int ray_num = rand() % 4;
		int led_num = rand() % 2;

		//Enabled sound or config option set?
		if ((sound_enabled && ((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) > LumenAudio.threshholds[kLumenAudioFrequency63Hz]) ||
									(LumenSettings.bass_boost && (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) > LumenAudio.threshholds[kLumenAudioFrequency160Hz])))) || size > 0) {
			LumenAudio.reset_max();
			//copy shifted flame template into array
			for (int i = 2; i < kSunBoardNumberOfLedsPerRay; i++) {
				//If the template is shifted down, then the top of the tube should be manually set.
				if ((i - led_num) < 0) {
					PatternHelpers.hue_next[0][i] = 0;
					PatternHelpers.hue_next[1][i] = 0;
					PatternHelpers.hue_next[2][i] = 0;
					PatternHelpers.hue_next[3][i] = 0;
				}
				else {
					PatternHelpers.hue_next[0][i] = (big_flame_hue[ray_num][i - led_num] + hue) % 360;
					PatternHelpers.hue_next[1][i] = (big_flame_hue[(ray_num + 1) % 4][i - led_num] + hue) % 360;
					PatternHelpers.hue_next[2][i] = (big_flame_hue[(ray_num + 2) % 4][i - led_num] + hue) % 360;
					PatternHelpers.hue_next[3][i] = (big_flame_hue[(ray_num + 3) % 4][i - led_num] + hue) % 360;
					PatternHelpers.value_next[0][i] = big_flame_value[ray_num][i - led_num];
					PatternHelpers.value_next[1][i] = big_flame_value[(ray_num + 1) % 4][i - led_num];
					PatternHelpers.value_next[2][i] = big_flame_value[(ray_num + 2) % 4][i - led_num];
					PatternHelpers.value_next[3][i] = big_flame_value[(ray_num + 3) % 4][i - led_num];
				}
			}
		}
		else {
			//copy shifted flame template into array
			for (int i = 2; i < kSunBoardNumberOfLedsPerRay; i++) {
				//If the template is shifted down, then the top of the tube should be manually set.
				if ((i - led_num) < 0) {
					PatternHelpers.hue_next[0][i] = 0;
					PatternHelpers.hue_next[1][i] = 0;
					PatternHelpers.hue_next[2][i] = 0;
					PatternHelpers.hue_next[3][i] = 0;
				}
				else {
					PatternHelpers.hue_next[0][i] = (flame_hue[ray_num][i - led_num] + hue) % 360;
					PatternHelpers.hue_next[1][i] = (flame_hue[(ray_num + 1) % 4][i - led_num] + hue) % 360;
					PatternHelpers.hue_next[2][i] = (flame_hue[(ray_num + 2) % 4][i - led_num] + hue) % 360;
					PatternHelpers.hue_next[3][i] = (flame_hue[(ray_num + 3) % 4][i - led_num] + hue) % 360;
					PatternHelpers.value_next[0][i] = flame_value[ray_num][i - led_num];
					PatternHelpers.value_next[1][i] = flame_value[(ray_num + 1) % 4][i - led_num];
					PatternHelpers.value_next[2][i] = flame_value[(ray_num + 2) % 4][i - led_num];
					PatternHelpers.value_next[3][i] = flame_value[(ray_num + 3) % 4][i - led_num];
				}
			}
		}
		//Determine how much we need to move each hue and value to reach the outcome in (255 / PatternHelpers.speed) number of steps.
		for (int i = 2; i < kSunBoardNumberOfLedsPerRay; i++) {
			for (int k = 0; k < 4; k++) {
				if ((PatternHelpers.hue[k][i] - PatternHelpers.hue_next[k][i]) > 180) {
					PatternHelpers.hue_next[k][i] += 360;
				}
				else if ((PatternHelpers.hue_next[k][i] - PatternHelpers.hue[k][i]) > 180) {
					PatternHelpers.hue[k][i] += 360;
				}
				hue_increment[k][i] = PatternHelpers.lerp_precise(PatternHelpers.hue[k][i], PatternHelpers.hue_next[k][i], PatternHelpers.speed) - PatternHelpers.hue[k][i];
				value_increment[k][i] = PatternHelpers.lerp_precise(PatternHelpers.value[k][i], PatternHelpers.value_next[k][i], PatternHelpers.speed) - PatternHelpers.value[k][i];
				PatternHelpers.hue[k][i] = PatternHelpers.hue[k][i] % 360;
				PatternHelpers.hue_next[k][i] = PatternHelpers.hue_next[k][i] % 360;
			}
		}
		PatternHelpers.progress = 0;
	}
	//Do the incrementing/decrementing and display new values.
	else {
		for (int i = 2; i < kSunBoardNumberOfLedsPerRay; i++) {
			PatternHelpers.hue[0][i] += hue_increment[0][i];
			PatternHelpers.hue[1][i] += hue_increment[1][i];
			PatternHelpers.hue[2][i] += hue_increment[2][i];
			PatternHelpers.hue[3][i] += hue_increment[3][i];
			PatternHelpers.value[0][i] += value_increment[0][i];
			PatternHelpers.value[1][i] += value_increment[1][i];
			PatternHelpers.value[2][i] += value_increment[2][i];
			PatternHelpers.value[3][i] += value_increment[3][i];
		}
		this->display();
		PatternHelpers.progress += PatternHelpers.speed;
	}
}

uint32_t CPatternFlame::get_pattern_id() {
	return 0x00000004;
}

void CPatternFlame::set_pattern_hue(int new_hue) {
	temp_hue = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternFlame::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternFlame::set_pattern_size(int new_size) {
	size = new_size;
}

void CPatternFlame::set_pattern_sound(bool is_sound_enabled) {
	sound_enabled = is_sound_enabled;
}

void CPatternFlame::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternFlameConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternFlameConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternFlameConfigSetSize: this->set_pattern_size((int)data); break;
		case kPatternFlameConfigSetSoundEnabled: this->set_pattern_sound((bool)data); break;
		}
}

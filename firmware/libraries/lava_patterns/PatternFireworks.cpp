/*
 * PatternFireworks.cpp
 *
 *  Created on: Feb 12, 2014
 *      Author: salem
 */

#include "PatternFireworks.h"
#include "PatternHelpers.h"
#include "LumenSettings.h"
#include "sun_board.h"
#include <cmath>
CPatternFireworks PatternFireworks;

void CPatternFireworks::start(LumenMoodConfig * configs, bool restore_to_defaults) {
		Lumen.leds->set_solid_rgb(0,0,0);
	if (configs == NULL) {
		PatternHelpers.erase_all();

		changes_ready = 0;
		PatternHelpers.hue[0][0] = 0;
		sound_enabled = false;
		hue_enabled = false;
		saved_power = 40;
		PatternHelpers.speed = 60;
		PatternHelpers.saturation[0][0] = 255;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.erase_all();
		changes_ready = 0;
		PatternHelpers.hue[0][0] = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		hue_enabled = configs->config_data[2].data;
		PatternHelpers.speed = configs->config_data[3].data;
		set_pattern_power(configs->config_data[4].data);
		sound_enabled = configs->config_data[5].data;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
	this->reset(0);
	this->reset(1);
	this->reset(2);
}

void CPatternFireworks::get_pattern_config(LumenMoodConfig * configs) {
	configs->config_data[0].config_id = kPatternFireworksConfigSetHue;
	configs->config_data[0].data = PatternHelpers.hue[0][0];
	configs->config_data[1].config_id = kPatternFireworksConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternFireworksConfigEnableHue;
	configs->config_data[2].data = hue_enabled;
	configs->config_data[3].config_id = kPatternFireworksConfigSetSpeed;
	configs->config_data[3].data = PatternHelpers.speed;
	configs->config_data[4].config_id = kPatternFireworksConfigSetPower;
	configs->config_data[4].data = 256 - saved_power;
	configs->config_data[5].config_id = kPatternFireworksConfigSetSoundEnabled;
	configs->config_data[5].data = sound_enabled;
}
//Clear values and choose new ray to shoot firework on.
void CPatternFireworks::reset(uint8_t firework_number) {
	Lumen.leds->set_level_rgb(0,0,0,0);
	Lumen.leds->set_level_rgb(1,0,0,0);
	Lumen.leds->set_level_rgb(2,0,0,0);
	if (hue_enabled) {
		hue[firework_number] = (330 + PatternHelpers.hue[0][0] + (rand() % 60)) % 360;
		saturation[firework_number] = PatternHelpers.value_cap((uint32_t)PatternHelpers.saturation[0][0] + (rand() % 60) - 30, 255, 0);
	}
	else {
		hue[firework_number] = rand() % 360;
		saturation[firework_number] = rand() % 255;
	}
	index[firework_number] = 10;
	progress[firework_number] = 255;
	ray_num[firework_number]= rand() % 4;

	temp_speed[firework_number] = PatternHelpers.speed * 2;
	pattern_state[firework_number] = kFireworksStateShooting;

}

void CPatternFireworks::draw_fireworks(uint8_t firework_number) {

	switch (pattern_type[firework_number]) {
			case kFireworksTypeExplosion : {
				Lumen.leds->set_pixel_hsv(ray_num[firework_number],0, hue[firework_number], saturation[firework_number], value[firework_number]);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,0, hue[firework_number], saturation[firework_number], value[firework_number]/2);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,0, hue[firework_number], saturation[firework_number], value[firework_number]/2);

				Lumen.leds->set_pixel_hsv(ray_num[firework_number],1, hue[firework_number], saturation[firework_number], value[firework_number]);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,1, hue[firework_number], saturation[firework_number], value[firework_number]);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,1, hue[firework_number], saturation[firework_number], value[firework_number]);

				Lumen.leds->set_pixel_hsv(ray_num[firework_number],2, hue[firework_number], saturation[firework_number], value[firework_number]);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,2, hue[firework_number], saturation[firework_number], value[firework_number]/2);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,2, hue[firework_number], saturation[firework_number], value[firework_number]/2);
				break;
			}
			case kFireworksTypeLines : {

				if (pattern_state[firework_number] == kFireworksStateFlamingBalls) {
					if (value[firework_number] > 190) {
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],3, (hue[firework_number] + 30) % 360, saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,3, (hue[firework_number] + 30) % 360, saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,3, (hue[firework_number] + 30) % 360, saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],4, (hue[firework_number] + 60) % 360, saturation[firework_number], value[firework_number]);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,4, (hue[firework_number] + 60) % 360, saturation[firework_number], value[firework_number]);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,4, (hue[firework_number] + 60) % 360, saturation[firework_number], value[firework_number]);
					}
					else if (value[firework_number] > 125) {
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],2, (hue[firework_number] + 15) % 360, saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,2, (hue[firework_number] + 15) % 360, saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,2, (hue[firework_number] + 10) % 360, saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],3, (hue[firework_number] + 30) % 360, saturation[firework_number], value[firework_number] * 3 /4);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,3, (hue[firework_number] + 30) % 360, saturation[firework_number], value[firework_number] * 3/4);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,3, (hue[firework_number] + 30) % 360, saturation[firework_number], value[firework_number] * 3 / 4);
					}
					else if (value[firework_number] > 62) {
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],1, hue[firework_number], saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,1, hue[firework_number], saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,1, hue[firework_number], saturation[firework_number], 0);
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],2, (hue[firework_number] + 15) % 360, saturation[firework_number], value[firework_number]/2);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,2, (hue[firework_number] + 15) % 360, saturation[firework_number], value[firework_number]/2);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,2, (hue[firework_number] + 10) % 360, saturation[firework_number], value[firework_number]/2);
					}
					else {
						Lumen.leds->set_pixel_hsv(ray_num[firework_number],1, hue[firework_number], saturation[firework_number], value[firework_number]/4);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,1, hue[firework_number], saturation[firework_number], value[firework_number]/4);
						Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,1, hue[firework_number], saturation[firework_number], value[firework_number]/4);
					}
				}
				else {
					Lumen.leds->set_pixel_hsv(ray_num[firework_number],4, (hue[firework_number] + 60) % 360, saturation[firework_number], value[firework_number]);
					Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,4, (hue[firework_number] + 60) % 360, saturation[firework_number], value[firework_number]);
					Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,4, (hue[firework_number] + 60) % 360, saturation[firework_number], value[firework_number]);
				}

				break;
			}
			case kFireworksTypeAssorted : {
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,0, (hue[firework_number] + (rand() % 60)) % 360, saturation[firework_number], value[firework_number]);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,0, (hue[firework_number] + (rand() % 60)) % 360, saturation[firework_number], value[firework_number]);

				Lumen.leds->set_pixel_hsv(ray_num[firework_number],1, hue[firework_number] , saturation[firework_number], value[firework_number]/2);

				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 1) % 4,2, (hue[firework_number] + (rand() % 60)) % 360, saturation[firework_number], value[firework_number]);
				Lumen.leds->set_pixel_hsv((ray_num[firework_number] + 3) % 4,2, (hue[firework_number] + (rand() % 60)) % 360, saturation[firework_number], value[firework_number]);
				break;
			}
		}
}

void CPatternFireworks::reporting(uint8_t firework_number) {
	//Decrease the brightness of the flaming balls based on what type of firework
	value[firework_number] = PatternHelpers.lerp(255, 0, progress[firework_number]);
	this->draw_fireworks(firework_number);
	progress[firework_number] += PatternHelpers.speed / 10;
	if (progress[firework_number] >= 255) {
		for(int i=0; i<3; i++) {
			PatternHelpers.value_next[ray_num[firework_number]][i] = 0;
		}
		this->reset(firework_number);
	}
}

void CPatternFireworks::flaming_balls(uint8_t firework_number) {
	//Increase the brightness of the flaming balls based on what type of firework
	Lumen.leds->set_solid_rgb(0,0,0);
	value[firework_number] = PatternHelpers.lerp(0, 255, progress[firework_number]);
	this->draw_fireworks(firework_number);
	if (pattern_type[firework_number] == kFireworksTypeLines) progress[firework_number] += PatternHelpers.speed /10;
	else progress[firework_number] += PatternHelpers.speed;
	if (progress[firework_number] >= 255) {
		progress[firework_number] = 0;
		pattern_state[firework_number] = kFireworksStateReporting;
	}

}

//See Halo pattern for more information on how this algorithm works.
void CPatternFireworks::shooting(uint8_t firework_number) {
	if(progress[firework_number] >= 255) {
			//reset moving index and tween progress[firework_number]
			progress[firework_number] = 0;
			index[firework_number] -= 1;
			temp_speed[firework_number] -= (temp_speed[firework_number]  / 6);
			//Once we reach the top, randomly choose a firework type.
			if(index[firework_number] == 0) {
				pattern_state[firework_number] = kFireworksStateFlamingBalls;
				pattern_type[firework_number] = (rand() % 2) + 1;
			}

			//lets copy PatternHelpers.value_next[0] into pattern
			for(int i=0; i<10; i++) {
				PatternHelpers.value[ray_num[firework_number]][i] = PatternHelpers.value_next[ray_num[firework_number]][i];
			}

			//then calculate pattern next
			for(int i=0; i<kSunBoardNumberOfLedsPerRay; i++) {
				int target = 0;
				if(i == index[firework_number]) target = 75;
				if(i == index[firework_number] + 1) target = 20;
				if(i == index[firework_number] + 2) target = 10;
				PatternHelpers.value_next[ray_num[firework_number]][i] = target;
			}
		}

		//lerp from pattern to PatternHelpers.value_next[0]
		for(int i = 2; i<kSunBoardNumberOfLedsPerRay; i++) {
			PatternHelpers.value[ray_num[firework_number]][i] = PatternHelpers.lerp(PatternHelpers.value[ray_num[firework_number]][i], PatternHelpers.value_next[ray_num[firework_number]][i], progress[firework_number]);
			Lumen.leds->set_pixel_hsv(ray_num[firework_number],i, 0, saturation[firework_number], PatternHelpers.value[ray_num[firework_number]][i]);
		}
		progress[firework_number] += temp_speed[firework_number];


}

//Every firework shoots, has flaming balls, and then reports.
void CPatternFireworks::step() {
	uint8_t buckets_passed_threshhold = 0;
	if (changes_ready == 3) {
			PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
			PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
			changes_ready = 0;
		}
	if (sound_enabled && ((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) > LumenAudio.threshholds[kLumenAudioFrequency63Hz]) ||
			(LumenSettings.bass_boost && (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) > LumenAudio.threshholds[kLumenAudioFrequency160Hz])))) {
		for (int i = 1; i < kLumenAudioFrequencyCount; i++) {
			if (LumenAudio.get_max_volume_bucket(i) > LumenAudio.threshholds[i]) {
				buckets_passed_threshhold++;
			}
		}
		this->set_pattern_speed(150 + 16 * buckets_passed_threshhold);
		power = 15 - buckets_passed_threshhold;
	}
	else if (sound_enabled) {
		power = 256;
	}
	else {
		power = saved_power;
	}
	LumenAudio.reset_max();

	for (int i = 0; i < 3; i++) {
		if (pattern_state[i] == kFireworksStateReporting) {
			this->reporting(i);
		}
	}
	for (int i = 0; i < 3; i++) {
		if (pattern_state[i] == kFireworksStateFlamingBalls) {
			this->flaming_balls(i);
		}
	}
	for (int i = 0; i < 3; i++) {
		if (pattern_state[i] == kFireworksStateShooting) {
			if (index[i] == 10) {
				if (ray_num[i] != ray_num[(i+1)%3] && ray_num[i] != ray_num[(i+2)%3]) {
					if (rand() % power == 0 && power != 256) {
						this->shooting(i);
					}
				}
				else {
					this->reset(i);
				}
			}
			else {
				this->shooting(i);
			}
		}

	}
}

void CPatternFireworks::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternFireworks::set_pattern_enable_hue(bool is_hue_enabled) {
	hue_enabled = is_hue_enabled;

}

void CPatternFireworks::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternFireworks::set_pattern_speed(int new_speed) {
	PatternHelpers.speed = new_speed;
	temp_speed[0] = new_speed;
	temp_speed[1] = new_speed;
	temp_speed[2] = new_speed;
	for (int i = 0; i < 9-index[0]; i++) {
		temp_speed[0] -= (temp_speed[0]  / 6);
	}
	for (int i = 0; i < 9-index[1]; i++) {
		temp_speed[1] -= (temp_speed[1]  / 6);
	}
	for (int i = 0; i < 9-index[2]; i++) {
		temp_speed[2] -= (temp_speed[2]  / 6);
	}
}

void CPatternFireworks::set_pattern_power(int new_power) {
	saved_power = 255 - new_power;
}

void CPatternFireworks::set_pattern_sound(bool is_sound_enabled) {
	sound_enabled = is_sound_enabled;
	if (!sound_enabled)
		power = saved_power;
}

uint32_t CPatternFireworks::get_pattern_id() {
	return 0x000000011;
}

void CPatternFireworks::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
	case kPatternFireworksConfigSetHue: this->set_pattern_hue((int)data); break;
	case kPatternFireworksConfigSetSaturation: this->set_pattern_saturation((int)data); break;
	case kPatternFireworksConfigSetSpeed: this->set_pattern_speed((int)data); break;
	case kPatternFireworksConfigEnableHue: this->set_pattern_enable_hue((bool)data); break;
	case kPatternFireworksConfigSetPower: this->set_pattern_power((int)data); break;
	case kPatternFireworksConfigSetSoundEnabled: this->set_pattern_sound((int)data); break;
	}
}

/*
 * PatternLantern.cpp
 *
 *  Created on: Feb 13, 2014
 *      Author: salem
 */

#include "PatternPlasma.h"
#include "PatternHelpers.h"
#include "sun_board.h"
#include "LumenSettings.h"
CPatternPlasma PatternPlasma;

void CPatternPlasma::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	LumenAudio.reset_max();
	changes_ready = 0;
	if (configs == NULL) {
		sound_enabled = 0;
		static_speed = 2;
		for (int i = 0; i < 40; i++) {
			// set base value of every pixel
			pixel_states[i] = 0;
		}
		PatternHelpers.hue[0][0] = 30;
		PatternHelpers.saturation[0][0] = 255;

	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.hue[0][0] = configs->config_data[0].data;
		PatternHelpers.saturation[0][0] = configs->config_data[1].data;
		set_pattern_speed(configs->config_data[2].data);
		sound_enabled = configs->config_data[3].data;
		for (int i = 0; i < 40; i++) {
			// set base value of every pixel
			pixel_states[i] = 0;
		}
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternPlasma::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternPlasmaConfigSetHue;
	configs->config_data[0].data = PatternHelpers.hue[0][0];
	configs->config_data[1].config_id = kPatternPlasmaConfigSetSaturation;
	configs->config_data[1].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternPlasmaConfigSetSpeed;
	configs->config_data[2].data = static_speed * 10;
	configs->config_data[3].config_id = kPatternPlasmaConfigSetSoundEnabled;
	configs->config_data[3].data = sound_enabled;
}

void CPatternPlasma::step() {

	if (changes_ready == 3) {
		PatternHelpers.hue[0][0] =  PatternHelpers.hue[0][1];
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}
	// pick a random pixel from 0-39
	int chosen_pixel = rand() % 40;

	// if this pixel is sitting in a base state, Plasma it
	if (pixel_states[chosen_pixel] == 0)
		pixel_states[chosen_pixel] = 255;

	if (sound_enabled && LumenAudio.get_max_volume_bucket(kLumenAudioFrequency1000Hz) > LumenAudio.threshholds[kLumenAudioFrequency1000Hz])
		PatternHelpers.hue[0][0] += 1;
	if (sound_enabled) {
		if (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) > LumenAudio.threshholds[kLumenAudioFrequency63Hz]) {
			PatternHelpers.speed = static_speed + PatternHelpers.value_cap((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) - LumenAudio.threshholds[kLumenAudioFrequency63Hz])/5, LumenSettings.bass_boost ? 10 : 20, 0);
		}
		else if (LumenSettings.bass_boost && (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) > LumenAudio.threshholds[kLumenAudioFrequency160Hz])) {
			PatternHelpers.speed += PatternHelpers.value_cap((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) - LumenAudio.threshholds[kLumenAudioFrequency160Hz]) / 5, 10, 0);
		}
		else
			PatternHelpers.speed = static_speed;
	}
	else
		PatternHelpers.speed = static_speed;
	if (PatternHelpers.speed > 50)
		PatternHelpers.speed = 50;
	PatternHelpers.hue[0][0] = PatternHelpers.hue[0][0] % 360;
	LumenAudio.reset_max();
	for (uint8_t i = 0; i < 40; i++) {
		// decay over time
		if (pixel_states[i] != 0) {
			// don't overflow
			if (PatternHelpers.speed > pixel_states[i])
				pixel_states[i] = 0;
			else
				pixel_states[i] -= PatternHelpers.speed;
		}

		uint8_t ray_id = i / 10;
		uint8_t led_id = i % 10;

		// are we Plasmaing up or down?
		if (pixel_states[i] >= 0 && pixel_states[i] <= 127) {
			int scalar = pixel_states[i];	// from 0 to 127

			// we need to be getting darker as we get closer to 0 (coming down from 127)
			Lumen.leds->set_pixel_linear_hsv(ray_id, led_id, PatternHelpers.hue[0][0] + (scalar / 7), PatternHelpers.saturation[0][0], PatternHelpers.value_cap(scalar+174, 255, 0));
		} else {
			int scalar = 127 - (pixel_states[i] - 128);	// from 0 to 127

			// we need to be getting brighter as we get closer to 127 (coming down from 255)
			Lumen.leds->set_pixel_linear_hsv(ray_id, led_id, PatternHelpers.hue[0][0] + (scalar / 7), PatternHelpers.saturation[0][0], PatternHelpers.value_cap(scalar+174, 255, 0));
		}
	}
}

void CPatternPlasma::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternPlasma::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternPlasma::set_pattern_speed(int new_speed) {
	new_speed = new_speed / 10;
	static_speed = new_speed; // 0 -50
}

void CPatternPlasma::set_pattern_sound(bool is_sound_enabled) {
	sound_enabled = is_sound_enabled;
}

uint32_t CPatternPlasma::get_pattern_id() {
	return 0x000000012;
}

void CPatternPlasma::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternPlasmaConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternPlasmaConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternPlasmaConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternPlasmaConfigSetSoundEnabled: this->set_pattern_sound((bool)data); break;
	}
}

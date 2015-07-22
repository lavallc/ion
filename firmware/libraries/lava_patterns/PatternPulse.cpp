/*
 * PatternPulse.cpp
 *
 *  Created on: Feb 20, 2014
 *      Author: salem
 */

#include "PatternPulse.h"
#include "PatternHelpers.h"
#include "LumenSettings.h"
#include "sun_board.h"

CPatternPulse PatternPulse;

void CPatternPulse::start(LumenMoodConfig * configs, bool restore_to_defaults) {

	frequency_hues[0] = 0;
	frequency_hues[1] = 0;
	frequency_hues[2] = 120;
	frequency_hues[3] = 240;
	frequency_hues[4] = 60;
	frequency_hues[5] = 180;
	frequency_hues[6] = 300;
	index = 9;
	PatternHelpers.erase_value();
	PatternHelpers.erase_hue();
	PatternHelpers.erase_saturation();
	PatternHelpers.progress = 255;
	if (configs == NULL) {
		PatternHelpers.speed = 255;
		sound_enabled = true;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.speed = configs->config_data[0].data;
		sound_enabled = configs->config_data[1].data;


	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}

void CPatternPulse::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternPulseConfigSetSpeed;
	configs->config_data[0].data = PatternHelpers.speed;
	configs->config_data[1].config_id = kPatternPulseConfigSetSoundEnabled;
	configs->config_data[1].data = sound_enabled;
}
void CPatternPulse::step() {
	if(PatternHelpers.progress >= 255) {
		//reset moving index and tween PatternHelpers.progress
		PatternHelpers.progress = 0;
		index--;
		if (index < 1)
			index = 1;


		//lets copy PatternHelpers.value_next[0] into pattern
		for(int i=0; i<10; i++) {
			PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
			PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
		}

		//then calculate pattern next
		for(int i=0; i<9; i++) {

			PatternHelpers.value_next[0][i] = PatternHelpers.value[0][i+1];
			PatternHelpers.hue_next[0][i] = PatternHelpers.hue[0][i+1];
		}
		if (sound_enabled) {
			PatternHelpers.value_next[0][9] = 0;
			if ((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) > LumenAudio.threshholds[kLumenAudioFrequency63Hz]) ||
					(LumenSettings.bass_boost && (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) > LumenAudio.threshholds[kLumenAudioFrequency160Hz]))) {
				uint8_t freq_max_position = 0;
				int freq_max_value = 0;
				int val = 0;
				int hue = 0;
				uint8_t buckets_passed_threshhold = 0;
				//If any buckets passed threshhold, see which bucket was the strongest after scaling.
				for (int i = 1; i < kLumenAudioFrequencyCount; i++) {
					if (LumenAudio.get_max_volume_bucket(i) > LumenAudio.threshholds[i]) {
						buckets_passed_threshhold++;
						val = (LumenAudio.get_max_volume_bucket(i) * PatternHelpers.define_colors[i].value) / 100;
						if (val > freq_max_value) {
							freq_max_position = i;
							freq_max_value = val;
						}
					}
				}

				//if ((party_mode && (buckets_passed_threshhold > 2 || brightness_peak  >=50)) || !party_mode) {
				//if(buckets_passed_threshhold > 1) {
					hue = frequency_hues[freq_max_position];

					//Goes through each frequency band and assigns the saturation to be scaled in reference to max freqency power
					//Then assigns the output hue to a combination of each frequency's base hue scaled by its saturation.
					//This again, all relative to the max frequency power and its defined hue.
					//See PatternHelpers.define_colors[] for more information.
					for (int i = 1; i < kLumenAudioFrequencyCount; i++) {
						if (i != freq_max_position) {
							int numerator = (PatternHelpers.threshhold_filter(LumenAudio.get_max_volume_bucket(i), LumenAudio.threshholds[i]) * PatternHelpers.define_colors[i].value) / 100;
							if (numerator > 1)
								PatternHelpers.bucket_chosen[i]--;
							if (PatternHelpers.bucket_chosen[i] < -20)
								PatternHelpers.bucket_chosen[i] = -20;
							if (PatternHelpers.bucket_chosen[i] <= -15 && PatternHelpers.define_colors[i].value < 100)
								PatternHelpers.define_colors[i].value++;
							numerator = (numerator * PatternHelpers.define_colors[i].value) / 100;

							PatternHelpers.define_colors[i].saturation =  (100 * numerator) / freq_max_value;

							hue = PatternHelpers.sum_audio_vector(hue, frequency_hues[freq_max_position], frequency_hues[i], PatternHelpers.define_colors[i].saturation);
						}
						else {
							if (buckets_passed_threshhold > 1)
								PatternHelpers.bucket_chosen[i]++;
							if (PatternHelpers.bucket_chosen[i] > 20)
									PatternHelpers.bucket_chosen[i] = 20;
							if (PatternHelpers.bucket_chosen[i] >= 15 && PatternHelpers.define_colors[i].value > 1)
								PatternHelpers.define_colors[i].value--;
						}
					}
					PatternHelpers.hue_next[0][9] = PatternHelpers.wrap_hue(hue);
					PatternHelpers.value_next[0][9] = 255;
				//}
			}
		}
		else {
			if (rand() % 5 == 0 && index < 8) {
				index = 9;
				PatternHelpers.value_next[0][9] = 255;
				PatternHelpers.hue_next[0][9] = rand() % 360;
			}
			else
				PatternHelpers.value_next[0][9] = 0;
		}
	}

	//lerp from pattern to PatternHelpers.value_next[0]
	for(int i = 0; i<kSunBoardNumberOfLedsPerRay; i++) {
		int b = PatternHelpers.lerp(PatternHelpers.value[0][i], PatternHelpers.value_next[0][i], PatternHelpers.progress);
		//int h = PatternHelpers.lerp(PatternHelpers.hue[0][i], PatternHelpers.hue_next[0][i], PatternHelpers.progress);
		Lumen.leds->set_level_hsv(i, PatternHelpers.hue_next[0][i], 255, b);
	}
	LumenAudio.reset_max();
	PatternHelpers.progress += PatternHelpers.speed;
}

void CPatternPulse::set_pattern_speed(int new_speed) {
	PatternHelpers.speed = new_speed;
}

void CPatternPulse::set_pattern_sound(bool is_sound_enabled) {
	sound_enabled = is_sound_enabled;
}

uint32_t CPatternPulse::get_pattern_id() {
	return 0x000000017;
}

void CPatternPulse::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
	case kPatternPulseConfigSetSpeed: this->set_pattern_speed((int)data); break;
	case kPatternPulseConfigSetSoundEnabled: this->set_pattern_sound((bool)data); break;
	}
}

/*
 * PatternWhirlpool.cpp
 *
 *  Created on: Feb 20, 2014
 *      Author: salem
 */

#include "PatternWhirlpool.h"
#include "PatternHelpers.h"
#include "sun_board.h"
#include "LumenSettings.h"
#include <cmath>
CPatternWhirlpool PatternWhirlpool;

void CPatternWhirlpool::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	PatternHelpers.erase_all();
	PatternHelpers.progress = 255;
	index = 2;

	lasthue = rand() % 360;
	hue_frequencies[4] = 60 ;
	hue_frequencies[5] = 180;
	hue_frequencies[6] = 300;
	for (int i = 0; i < 10; i++) {
		PatternHelpers.hue_next[0][i] = lasthue;
		PatternHelpers.hue_next[1][i] = lasthue;
		PatternHelpers.hue_next[2][i] = lasthue;
		PatternHelpers.hue_next[3][i] = lasthue;
		PatternHelpers.value_next[0][i] = 76;
		PatternHelpers.value_next[1][i] = 76;
		PatternHelpers.value_next[2][i] = 76;
		PatternHelpers.value_next[3][i] = 76;
	}

	if (configs == NULL) {
		hue_frequencies[1] = 0 ;
		hue_frequencies[2] = 120;
		hue_frequencies[3] = 240;
		whirlpool_saturation = 255;
		static_speed = 125;
		if (restore_to_defaults)
			LumenSettings.bass_boost = true;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		static_speed = configs->config_data[0].data;
		hue_frequencies[1] = configs->config_data[1].data;
		hue_frequencies[2] = (hue_frequencies[1] + 120) % 360;
		hue_frequencies[3] = (hue_frequencies[1] + 240) % 360;
		whirlpool_saturation = configs->config_data[2].data;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}

}
void CPatternWhirlpool::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].data = static_speed;
	configs->config_data[0].config_id = kPatternWhirlpoolConfigSetSpeed;
	configs->config_data[1].data = hue_frequencies[1];
	configs->config_data[1].config_id = kPatternWhirlpoolConfigSetHue;
	configs->config_data[2].data = whirlpool_saturation;
	configs->config_data[2].config_id = kPatternWhirlpoolConfigSetSaturation;
	configs->config_data[3].data = LumenSettings.bass_boost;
	Serial.debug_print("bass");
	Serial.debug_print(LumenSettings.bass_boost);
	configs->config_data[3].config_id = kPatternWhirlpoolConfigSetBassBoost;
}

void CPatternWhirlpool::spread(uint8_t ray, uint8_t led) {
	if (led > 0) {
		if ((PatternHelpers.hue_next[ray][led] - PatternHelpers.hue_next[ray][led-1]) > 180) {
			PatternHelpers.hue_next[ray][led-1] += 360;
		}
		else if ((PatternHelpers.hue_next[ray][led-1] - PatternHelpers.hue_next[ray][led]) > 180) {
			PatternHelpers.hue_next[ray][led] += 360;
		}
	}
	//PatternHelpers.hue_next[ray][led] += ((PatternHelpers.hue_next[ray][led] - PatternHelpers.hue_next[ray][led-1]));// * (rand() % 50)) / 50;
	//PatternHelpers.hue_next[ray][led] = PatternHelpers.hue_next[ray][led] % 360;
	PatternHelpers.hue_next[ray][led] = PatternHelpers.hue_next[ray][led-1] - PatternHelpers.hue_next[ray][led];
	int strength  = (rand() % 41);
	strength += 10;
	PatternHelpers.hue_next[ray][led] = (PatternHelpers.hue_next[ray][led] * strength) / 50;
	PatternHelpers.hue_next[ray][led] += (PatternHelpers.hue[ray][led] + 360);
	PatternHelpers.hue_next[ray][led] = PatternHelpers.hue_next[ray][led] % 360;

	int movement = PatternHelpers.value_next[ray][led-1] - PatternHelpers.value_next[ray][led];
	movement = ((movement * (int)PatternHelpers.speed) / 5) / 50;
	PatternHelpers.value_next[ray][led] += movement;
}

void CPatternWhirlpool::step() {


	if (changes_ready == 3) {
		hue_frequencies[1] = new_hue ;
		hue_frequencies[2] = (new_hue  + 120) % 360;
		hue_frequencies[3] = (new_hue  + 240) % 360;
		whirlpool_saturation = new_saturation;
		changes_ready = 0;
	}
	uint8_t buckets_passed_threshhold = 0;
	uint8_t freq_max_position = 0;
	int freq_max_value = 0;
	int val = 0;
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
	int bass_val = 0;
	int low_val = 0;
	if (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) > LumenAudio.threshholds[kLumenAudioFrequency63Hz]) {
		bass_val = PatternHelpers.value_cap((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz)-LumenAudio.threshholds[kLumenAudioFrequency63Hz]) , 255, 0);
		if ((brightness_peak < 50) && (buckets_passed_threshhold >= 1))
			brightness_peak = 100;
	}
	if (LumenSettings.bass_boost && (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) > LumenAudio.threshholds[kLumenAudioFrequency160Hz])) {
		low_val = PatternHelpers.value_cap((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz)-LumenAudio.threshholds[kLumenAudioFrequency160Hz]) , 255, 0);
		if ((brightness_peak < 50) && (buckets_passed_threshhold >= 1))
			brightness_peak = 100;
	}
	PatternHelpers.speed = PatternHelpers.value_cap((int)static_speed * 0.8 +  (bass_val + low_val) * 0.2, 255, 1);
	if(PatternHelpers.progress >= 255) {
		//reset moving index and tween PatternHelpers.progress
		PatternHelpers.progress = PatternHelpers.progress % 255;
		index++;
		for (int i = 0; i < 10; i++) {
			PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
			PatternHelpers.hue[1][i] = PatternHelpers.hue_next[1][i];
			PatternHelpers.hue[2][i] = PatternHelpers.hue_next[2][i];
			PatternHelpers.hue[3][i] = PatternHelpers.hue_next[3][i];
			PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
			PatternHelpers.value[1][i] = PatternHelpers.value_next[1][i];
			PatternHelpers.value[2][i] = PatternHelpers.value_next[2][i];
			PatternHelpers.value[3][i] = PatternHelpers.value_next[3][i];

		}
		if (index == 3) {
			if (brightness_peak > 50) {
				PatternHelpers.value_next[0][0] = 74 + buckets_passed_threshhold * 29;
				PatternHelpers.value_next[1][0] = 74 + buckets_passed_threshhold * 29;
				PatternHelpers.value_next[2][0] = 74 + buckets_passed_threshhold * 29;
				PatternHelpers.value_next[3][0] = 74 + buckets_passed_threshhold * 29;
				kick_detected = true;
			}
			index = 0;
			//if (buckets_passed_threshhold > 1 || brightness_peak  >=50 ) {
				lasthue = hue;
				hue = hue_frequencies[freq_max_position];
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

						hue = PatternHelpers.sum_audio_vector(hue, hue_frequencies[freq_max_position], hue_frequencies[i], PatternHelpers.define_colors[i].saturation);

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
				//When doing the filter, we need to find the shortest route between hue and last_hue.
				hue = PatternHelpers.wrap_hue(hue);
				int transition_strength = 20;
				if (brightness_peak == 100) {
					transition_strength = 35;
				}
				if ((hue - PatternHelpers.hue[0][1]) > 180) {
					hue = 360 + PatternHelpers.hue[0][1]  - ((transition_strength * (360 + PatternHelpers.hue[0][1] - hue)) / 100) ;
				}
				else if ((PatternHelpers.hue[0][1] - hue) > 180) {
					hue = ((transition_strength * (360 + hue - PatternHelpers.hue[0][1])) / 100) + PatternHelpers.hue[0][1];
				}
				else {
					hue = ((transition_strength * (hue - PatternHelpers.hue[0][1])) / 100) + PatternHelpers.hue[0][1];
				}
				hue = PatternHelpers.wrap_hue(hue);

			if (kick_detected) {
				kick_detected = false;
			}
			else {
				PatternHelpers.value_next[0][0] = 76;
				PatternHelpers.value_next[1][0] = 76;
				PatternHelpers.value_next[2][0] = 76;
				PatternHelpers.value_next[3][0] = 76;
			}
			PatternHelpers.hue_next[0][0] = hue;
			PatternHelpers.hue_next[1][0] = hue;
			PatternHelpers.hue_next[2][0] = hue;
			PatternHelpers.hue_next[3][0] = hue;
			for (int k = 0; k < 4; k++) {
				for(int i = 9; i>1; i--) {
					this->spread(k,i);
				}
			}
		}
		else {
			for (int k = 0; k < 4; k++) {
				for(int i = 9; i>0; i--) {
					this->spread(k,i);
				}
			}
		}
	}
	brightness_peak -= (PatternHelpers.speed/5);
	if (brightness_peak < 30) brightness_peak = 30;
	//lerp from pattern to PatternHelpers.value_next[0]
	for (int k = 0; k < 4; k++) {
		for(int i = 0; i<kSunBoardNumberOfLedsPerRay; i++) {
			if (PatternHelpers.hue_next[k][i] - PatternHelpers.hue[k][i] > 180 && PatternHelpers.hue[k][i] < PatternHelpers.hue_next[k][i])
				PatternHelpers.hue[k][i] += 360;
			if (PatternHelpers.hue[k][i] - PatternHelpers.hue_next[k][i] > 180 && PatternHelpers.hue_next[k][i] < PatternHelpers.hue[k][i])
				PatternHelpers.hue_next[k][i] += 360;
			uint16_t h = PatternHelpers.lerp(PatternHelpers.hue[k][i], PatternHelpers.hue_next[k][i], PatternHelpers.progress);
			uint8_t v = PatternHelpers.lerp(PatternHelpers.value[k][i], PatternHelpers.value_next[k][i], PatternHelpers.progress);
			Lumen.leds->set_pixel_hsv(k,i, h % 360, whirlpool_saturation, v);
		}
	}

	PatternHelpers.progress += PatternHelpers.speed;
	LumenAudio.reset_max();
}


void CPatternWhirlpool::set_pattern_speed(int new_speed) {
		static_speed = new_speed;
}

void CPatternWhirlpool::set_pattern_hue(int hue_value) {
	new_hue = hue_value;
	changes_ready = changes_ready | 0x01;
}

void CPatternWhirlpool::set_pattern_saturation(int sat_value) {
	new_saturation = sat_value;
	changes_ready = changes_ready | 0x02;
}

void CPatternWhirlpool::set_pattern_bass_boost(bool new_bass_boost) {
	LumenSettings.bass_boost = new_bass_boost;
}

uint32_t CPatternWhirlpool::get_pattern_id() {
	return 0x000000015;
}

void CPatternWhirlpool::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
	case kPatternWhirlpoolConfigSetSpeed: this->set_pattern_speed((int)data); break;
	case kPatternWhirlpoolConfigSetHue: this->set_pattern_hue((int)data); break;
	case kPatternWhirlpoolConfigSetSaturation: this->set_pattern_saturation((int)data); break;
	case kPatternWhirlpoolConfigSetBassBoost: this->set_pattern_bass_boost((bool)data); break;
	}
}

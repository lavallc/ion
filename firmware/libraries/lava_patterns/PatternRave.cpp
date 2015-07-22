
/*
 * PatternRave.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: Salemancer
 *      Pattern Creator: Salemancer
 */
#include "PatternHelpers.h"
#include "PatternRave.h"
#include "LumenSettings.h"
CPatternRave PatternRave;


void CPatternRave::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	delay = 0;
	if (configs == NULL) {
		PatternHelpers.speed = 5;
		PatternHelpers.define_colors[1].hue = 0;
		PatternHelpers.define_colors[2].hue = 120;
		PatternHelpers.define_colors[3].hue = 360;
		hue = 0;
		PatternHelpers.saturation[0][0] = 255;
		if (restore_to_defaults)
			LumenSettings.bass_boost = true;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		set_pattern_speed(configs->config_data[0].data);
		hue = configs->config_data[1].data;
		PatternHelpers.define_colors[1].hue = hue;
		PatternHelpers.define_colors[2].hue = (hue + 120) % 360;
		PatternHelpers.define_colors[3].hue = (hue + 240) % 360;
		PatternHelpers.saturation[0][0] = configs->config_data[2].data;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}
void CPatternRave::get_pattern_config(LumenMoodConfig * configs){
	if (PatternHelpers.speed == 1)
		configs->config_data[0].data = 9;
	else if (PatternHelpers.speed == 2)
		configs->config_data[0].data = 24;
	else if (PatternHelpers.speed == 3)
		configs->config_data[0].data = 49;
	else if (PatternHelpers.speed == 4)
		configs->config_data[0].data = 74;
	else if (PatternHelpers.speed == 5)
		configs->config_data[0].data = 99;
	else if (PatternHelpers.speed == 25)
		configs->config_data[0].data = 255;
	else
		configs->config_data[0].data = (PatternHelpers.speed * 17);
	configs->config_data[0].data = 255 - configs->config_data[0].data;
	configs->config_data[0].config_id = kPatternRaveConfigSetSpeed;

	configs->config_data[1].data = PatternHelpers.define_colors[1].hue;
	configs->config_data[1].config_id = kPatternRaveConfigSetHue;
	configs->config_data[2].data = PatternHelpers.saturation[0][0];
	configs->config_data[2].config_id = kPatternRaveConfigSetSaturation;
	configs->config_data[3].data = LumenSettings.bass_boost;
	Serial.debug_print("bass");
		Serial.debug_print(LumenSettings.bass_boost);
	configs->config_data[3].config_id = kPatternRaveConfigSetBassBoost;
}

void CPatternRave::step() {


	if (changes_ready == 3) {
		PatternHelpers.define_colors[kLumenAudioFrequency160Hz].hue = PatternHelpers.hue[0][0] ;
		PatternHelpers.define_colors[kLumenAudioFrequency400Hz].hue = (PatternHelpers.hue[0][0]  + 120) % 360;
		PatternHelpers.define_colors[kLumenAudioFrequency1000Hz].hue = (PatternHelpers.hue[0][0]  + 240) % 360;
		PatternHelpers.saturation[0][0] =PatternHelpers.saturation[0][1];
		changes_ready = 0;
	}
	//if (value < 50) value = 50;
	//Delay between changing the hue(150ms)
	if (delay < 1) {
		delay = PatternHelpers.speed;
		int val = 0;
		uint8_t buckets_passed_threshhold = 0;
		uint8_t freq_max_position = 0;
		int freq_max_value = 0;
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

		if ((LumenAudio.get_max_volume_bucket(kLumenAudioFrequency63Hz) > LumenAudio.threshholds[kLumenAudioFrequency63Hz])
				&& (brightness_peak < 50)) {
			brightness_peak = 100;
		}
		if (LumenSettings.bass_boost && (LumenAudio.get_max_volume_bucket(kLumenAudioFrequency160Hz) > LumenAudio.threshholds[kLumenAudioFrequency160Hz])
				&& (brightness_peak < 50)) {
			brightness_peak = 100;
		}

		if (buckets_passed_threshhold >= 1) {
		//if(buckets_passed_threshhold >= 1 || brightness_peak > 20) {
			lasthue = hue;
			hue = PatternHelpers.define_colors[freq_max_position].hue;

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

					hue = PatternHelpers.sum_audio_vector(hue, PatternHelpers.define_colors[freq_max_position].hue, PatternHelpers.define_colors[i].hue, PatternHelpers.define_colors[i].saturation);
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

			int transition_strength = 10;
			hue = PatternHelpers.wrap_hue(hue);
			if (brightness_peak == 100) {
				transition_strength = 85;
			}
			if ((hue - lasthue) > 180) {
				hue = 360 + lasthue  - ((transition_strength * (360 + lasthue - hue)) / 100) ;
			}
			else if ((lasthue - hue) > 180) {
				hue = ((transition_strength * (360 + hue - lasthue)) / 100) + lasthue;
			}
			else {
				hue = ((transition_strength * (hue - lasthue)) / 100) + lasthue;
			}
		}
	}
	LumenAudio.reset_max();
/*
	if (last_brightness_peak > 40 && last_brightness_peak < 50 && brightness_peak > 50 && party_mode) {
	//Scales brightness_peak of 0-100 to brightness_step of -3-11 to create an index for the pulse ring to move with.
		int brightness_step = brightness_peak - 20;
		 brightness_step = ((brightness_step * 14) / 80) - 3;
		for (int i = 0; i < 10; i++) {
			if ((int)brightness_step + 1 == i || (int)brightness_step == i || (int)brightness_step - 1 == i || (int)brightness_step + 2 == i || (int)brightness_step - 2 == i)
				Lumen.leds->set_level_hsv(i,hue, 255, 255);
			else
				Lumen.leds->set_level_hsv(i,hue, 255, 20 * 2.55);
		}

	}
	else {*/
		Lumen.leds->set_solid_hsv(hue, PatternHelpers.saturation[0][0], brightness_peak * 2.55);
	//}

	brightness_peak -= (13 - PatternHelpers.speed/2);
	if (brightness_peak < 20) brightness_peak = 20;
	if (brightness_peak < 50) last_brightness_peak = brightness_peak;
	if (delay != 0) delay--;

}

void CPatternRave::set_pattern_hue(int hue_value) {
	PatternHelpers.hue[0][0] = hue_value;
	changes_ready = changes_ready | 0x01;
}

void CPatternRave::set_pattern_saturation(int sat_value) {
	PatternHelpers.saturation[0][1] = sat_value;
	changes_ready = changes_ready | 0x02;
}

void CPatternRave::set_pattern_speed(int new_speed) {
	new_speed = 250 - new_speed;
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
	else
		PatternHelpers.speed = (new_speed / 17);
}

void CPatternRave::set_pattern_bass_boost(bool new_bass_boost) {
	LumenSettings.bass_boost = new_bass_boost;
}

uint32_t CPatternRave::get_pattern_id() {
	return 0x00000007;
}

void CPatternRave::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternRaveConfigSetSpeed: this->set_pattern_speed((int)data); break;
		case kPatternRaveConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternRaveConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternRaveConfigSetBassBoost: this->set_pattern_bass_boost((bool)data); break;
	}
}


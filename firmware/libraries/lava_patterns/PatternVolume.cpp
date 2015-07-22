
/*
 * PatternVolume.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: Salemancer
 *      Pattern Creator: Salemancer
 */
#include "PatternHelpers.h"
#include "PatternVolume.h"
#include "LumenAudio.h"
#include <cmath>
CPatternVolume PatternVolume;

void CPatternVolume::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	sum = 0;
	PatternHelpers.speed = 2;
	sensitivity = 20;
	brightness = (uint16_t)(215 / sensitivity);
	bucket = 8;
	if (configs == NULL) {

	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}

}
void CPatternVolume::get_pattern_config(LumenMoodConfig * configs){

}

void CPatternVolume::step() {
	sum += PatternHelpers.value_cap((LumenAudio.get_max_volume_bucket(0)-135) * 2 / 3, 65535, 0);
	sum += PatternHelpers.value_cap((LumenAudio.get_max_volume_bucket(1)-140) * 2 / 3, 65535, 0);
	sum += PatternHelpers.value_cap(LumenAudio.get_max_volume_bucket(2)-125, 65535, 0);
	sum += PatternHelpers.value_cap(LumenAudio.get_max_volume_bucket(3)-125, 65535, 0);
	sum += PatternHelpers.value_cap(LumenAudio.get_max_volume_bucket(4)-125, 65535, 0);

	delay = PatternHelpers.speed;

	int vals = sqrt(sum * 67);
	LumenLeds.set_level_hsv(0,0,255, vals > sensitivity * 10 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 10) % sensitivity) *12);
	LumenLeds.set_level_hsv(1,0,255, vals > sensitivity * 9 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 9) % sensitivity) *12);
	LumenLeds.set_level_hsv(2,60,255, vals > sensitivity * 8 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 8) % sensitivity) *12);
	LumenLeds.set_level_hsv(3,60,255, vals > sensitivity * 7 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 7) % sensitivity) *12);
	LumenLeds.set_level_hsv(4,120,255, vals > sensitivity * 6 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 6) % sensitivity) *12);
	LumenLeds.set_level_hsv(5,120,255, vals > sensitivity * 5 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 5) % sensitivity) *12);
	LumenLeds.set_level_hsv(6,120,255, vals > sensitivity * 4 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 4) % sensitivity) *12);
	LumenLeds.set_level_hsv(7,120,255, vals > sensitivity * 3 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 3) % sensitivity) *12);
	LumenLeds.set_level_hsv(8,120,255, vals > sensitivity * 2 ? 255 : (PatternHelpers.threshhold_filter(vals, sensitivity * 2) % sensitivity) *12);
	LumenLeds.set_level_hsv(9,120,255,255);
	sum = PatternHelpers.value_cap(sum - 30, 603,0);

	LumenAudio.reset_max();

}

uint32_t CPatternVolume::get_pattern_id() {
	return 0x00000018;
}

void CPatternVolume::handle_config_data(uint8_t config_id, uint32_t data) {

}

/*
 * PatternHelpers.h
 *
 *  Created on: Jan 30, 2014
 *      Author: billy
 */

#ifndef PATTERNHELPERS_H_
#define PATTERNHELPERS_H_
#include "IPattern.h"
#include "LumenAudio.h"
class CPatternHelpers {
public:
	int lerp(int v0, int v1, int t);
	int lerp_precise(int v0, int v1, int t);
	int value_cap(int value, int cap, int bottom);
	void erase_value();
	void erase_hue();
	void erase_saturation();
	void erase_all();
	int wrap_hue(int hue);
	int threshhold_filter(int value, int threshhold);
	int sum_audio_vector(int current_hue, int max_hue, int new_hue, int new_strength);
	uint8_t speed;
	uint16_t progress;
	uint8_t value[4][10];
	uint8_t value_next[4][10];
	int16_t hue[4][10];
	int16_t hue_next[4][10];
	uint8_t saturation[4][10];
	uint8_t saturation_next[4][10];
	int8_t bucket_chosen[7] = {0,0,0,0,0,0,0};
	define_color_t define_colors[kLumenAudioFrequencyCount];
};

extern CPatternHelpers PatternHelpers;

#endif /* PATTERNHELPERS_H_ */

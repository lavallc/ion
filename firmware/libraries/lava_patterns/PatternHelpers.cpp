/*
 * PatternHelpers.cpp
 *
 *  Created on: Jan 30, 2014
 *      Author: billy
 */

#include "PatternHelpers.h"
#include "sun_board.h"
#include "Lumen.h"
#include <cmath>
extern "C" {
	#include "nrf_gpio.h"
}

CPatternHelpers PatternHelpers;

//Standard lerp function, use this
int CPatternHelpers::lerp(int v0, int v1, int t) {
	int b = v0+(v1-v0)*t/255;
  return b;
}

//For lerping very small amounts, this always rounds up(used in flame)
int CPatternHelpers::lerp_precise(int v0, int v1, int t) {
	int b = (v1-v0)*t;
	if (b > 0)
		b = v0 + ceil((float)b/255.0);
	else
		b = v0 + floor((float)b/255.0);
  return b;
}

//Used to prevent math operations from overflowing the data type.
int CPatternHelpers::value_cap(int value, int cap, int bottom) {
	if (value > cap)
		return cap;
	else if (value < bottom)
		return bottom;
	else
		return value;
}

//Used to shift hue if it goes negative from a calculation
int CPatternHelpers::wrap_hue(int hue_value) {
	if (hue_value > 359)
		return hue_value % 360;
	if (hue_value < 0) {
		hue_value += 360;
	}
	return hue_value;
}

//used for audio to remove the noise floor
int CPatternHelpers::threshhold_filter(int value, int threshhold) {
	if (value < threshhold)
		return 1;
	else
		return value;
}



void CPatternHelpers::erase_value() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 10; j++) {
			value[i][j] = 0;
			value_next[i][j] = 0;
		}
	}
}
void CPatternHelpers::erase_hue() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 10; j++) {
			hue[i][j] = 0;
			hue_next[i][j] = 0;
		}
	}
}
void CPatternHelpers::erase_saturation() {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 10; j++) {
			saturation[i][j] = 0;
			saturation_next[i][j] = 0;
		}
	}
}
void CPatternHelpers::erase_all() {

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 10; j++) {
			value[i][j] = 0;
			value_next[i][j] = 0;
			saturation[i][j] = 0;
			saturation_next[i][j] = 0;
			hue[i][j] = 0;
			hue_next[i][j] = 0;
		}
	}
	this->progress = 0;
	this->speed = 0;
}

int CPatternHelpers::sum_audio_vector(int current_hue, int max_hue, int new_hue, int new_strength) {
	if (max_hue == 0 && new_hue > 180) {
		current_hue = current_hue
				- (((360 - new_hue) * new_strength) / 100);
	} else if (new_hue == 0 && max_hue > 180) {
		current_hue = current_hue
				+ (((360 - max_hue) * new_strength) / 100);
	} else if (new_hue < max_hue) {
		current_hue = current_hue
				- (((max_hue - new_hue) * new_strength) / 100);
	} else {
		current_hue = current_hue
				+ (((new_hue - max_hue) * new_strength) / 100);
	}
	return current_hue;
}

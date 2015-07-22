/*
 * PatternIon.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: salem
 */

#include "PatternIon.h"
#include "PatternHelpers.h"
#include "sun_board.h"
#include <cmath>
CPatternIon PatternIon;

void CPatternIon::start(LumenMoodConfig * configs, bool restore_to_defaults) {
	PatternHelpers.erase_all();
	index[0] = 8;
	index[1] = 6;
	index[2] = 4;
	index[3] = 2;
	progress[0] = 255;
	progress[1] = 255;
	progress[2] = 255;
	progress[3] = 255;
	direction[0] = -1; //up
	direction[1] = -1;
	direction[2] = -1;
	direction[3] = -1;
	hue = 240;
	speed[0] = 255;
	speed[1] = 255;
	speed[2] = 255;
	speed[3] = 255;
}

void CPatternIon::get_pattern_config(LumenMoodConfig * configs) {

}
void CPatternIon::step() {
	for (int k = 0; k < 4; k++) {
		if(progress[k] >= 255) {

			//reset moving index and tween progress
			progress[k] = 0;
			index[k] += 1*direction[k];
			if(index[k] > 9 || index[k] < 0) {
			//	speed[k]  += (rand() % 50);
		//		speed[k] = (speed[k] % 50) + 50;
				direction[k] *= -1;
				index[k] += 2*direction[k];
			}

			//lets copy PatternHelpers.value_next[0] into pattern
			for(int i=1; i<9; i++) {
				PatternHelpers.value[k][i] = PatternHelpers.value_next[k][i];
			}

			//then calculate pattern next
			for(int i=1; i<9; i++) {
				int target = 255;
				if(i == index[k]) target = 125;
				if(i == index[k] - direction[k] && speed[k] > 178) target = 165;
				if(i == index[k] - 2*direction[k] && speed[k] > 230) target = 200;
				PatternHelpers.value_next[k][i] = target;
			}
		}
	}
	for (int k = 0; k < 4; k++) {
		//lerp from pattern to PatternHelpers.value_next[0]
		for(int i = 1; i<9; i++) {
			int s = PatternHelpers.lerp(PatternHelpers.value[k][i], PatternHelpers.value_next[k][i], progress[k]);
			Lumen.leds->set_pixel_hsv(k,i, hue, s, 153);
		}
	}

	for (int k = 0; k < 4; k++) {
		progress[k] += speed[k];
	}
	Lumen.leds->set_level_hsv(0, 240, 255, 255);
	Lumen.leds->set_level_hsv(9, 240, 255, 255);

}

void CPatternIon::set_pattern_hue(int new_hue) {
	hue = new_hue;
}

void CPatternIon::set_pattern_speed(int new_speed) {
	speed[0] = new_speed;
}

uint32_t CPatternIon::get_pattern_id() {
	return 0x000000014;
}

void CPatternIon::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
	case kPatternIonConfigSetHue: this->set_pattern_hue((int)data); break;
	case kPatternIonConfigSetSpeed: this->set_pattern_speed((int)data); break;
	}
}

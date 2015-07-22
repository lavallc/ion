/*
 * PatternLava.h
 *
 *  Created on: Feb 11, 2014
 *      Author: billy
 */

#ifndef PATTERNLAVA_H_
#define PATTERNLAVA_H_

#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternLavaConfigSetSpeed = 0x01,
	kPatternLavaConfigSetHue,
	kPatternLavaConfigSetSaturation,
} kPatternLavaConfig;

class CPatternLava : public IPattern {
private:
	void set_pattern_speed(int new_speed);
	void set_pattern_hue(int new_hue);
	void set_pattern_saturation(int new_saturation);
	void set_pattern_sound(bool is_sound_enabled);
	int8_t position= 0;
	int8_t direction[4] = {0,0,0,0};
	int8_t led_num[4] = {9,9,9,9};
	int8_t ray_num[4] = {8,8,8,8};
	uint8_t changes_ready;
	const unsigned char lava_value[4][3] = {
			 {100,	255,	100},
			 {0,	100,	0},
			 {0,	0,	    0},
			 {0,	100,	0}
		};
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};

extern CPatternLava PatternLava;

#endif /* PATTERNLAVA_H_ */

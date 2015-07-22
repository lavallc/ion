/*
 * PatternFlame.h
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#ifndef PATTERNFLAME_H_
#define PATTERNFLAME_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternFlameConfigSetHue = 0x01,
	kPatternFlameConfigSetSaturation,
	kPatternFlameConfigSetSize,
	kPatternFlameConfigSetSoundEnabled,
} kPatternFlameConfig;

class CPatternFlame : public IPattern {
private:
	int16_t hue_increment[4][10];
	int16_t value_increment[4][10];
	uint8_t size;
	uint16_t hue;
	uint16_t temp_hue;
	uint8_t changes_ready;
	bool sound_enabled;

	const unsigned short flame_hue[4][10] = {
		 {0,0,0,0,0,	0,	5,	13,	25,	48},
		 {0,0,0,0,0,	0,	0,	13,	16,	35},
		 {0,0,0,0,0,	0,	0,	5,	8,	30},
		 {0,0,0,0,0,	0,	0,	13,	16,	35}
	};
	const unsigned char flame_value[4][10] = {
		 {0,0,0,0,0,	30,	70,	130,	150,	170},
		 {0,0,0,0,0,	0,	30,	70,		130,	150},
		 {0,0,0,0,0,	0,	10,	30,		70,		130},
		 {0,0,0,0,0,	0,	30,	70,		130,	150}
	};
//Need to convert these to 255 scale
	const unsigned short big_flame_hue[4][10] = {
		 {0,0,0,0,5,	10,	13,	20,	30,	48},
		 {0,0,0,0,0,	13,	18,	20,	22,	35},
		 {0,0,0,0,0,	8,	10,	15,	20,	30},
		 {0,0,0,0,0,	13,	18,	20,	22,	35}
	};
	const unsigned char big_flame_value[4][10] = {
		 {0,0,30,70,130,	150,	170,	190,	215,	255},
		 {0,0,0,30,70,	130,	150,	170,	190,	215},
		 {0,0,0,10,30,	70,	130,	150,	170,	190},
		 {0,0,0,30,70,	130,	150,	170,	190,	215}
	};

	void set_pattern_size(int new_size);
	void set_pattern_hue(int new_hue);
	void set_pattern_sound(bool is_sound_enabled);
	void set_pattern_saturation(int new_saturation);
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void display();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternFlame PatternFlame;

#endif /* PATTERNFLAME_H_ */

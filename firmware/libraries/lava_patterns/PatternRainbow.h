/*
 * PatternMatrix.h
 *
 *  Created on: Jan 31, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNRAINBOW_H_
#define PATTERNRAINBOW_H_
#include <cstdint>
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternRainbowConfigSetSpeed = 0x01,
	kPatternRainbowConfigSetBrightness,
	kPatternRainbowConfigSetReverse,
} kPatternRainbowConfig;

class CPatternRainbow : public IPattern {
private:
	int16_t hue = 0;
	uint8_t value;
	int8_t is_reverse;
	void set_pattern_speed(int new_speed);
	void set_pattern_brightness(int new_brightness);
	void set_pattern_reverse(int set_reverse);

public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);

};

extern CPatternRainbow PatternRainbow;

#endif /* PATTERNMATRIX_H_ */

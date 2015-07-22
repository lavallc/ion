/*
 * PatternSpiral.h
 *
 *  Created on: Jan 26, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNSpiral_H_
#define PATTERNSpiral_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternSpiralConfigSetHue = 0x01,
	kPatternSpiralConfigSetSaturation,
	kPatternSpiralConfigSetSpeed,
	kPatternSpiralConfigSetBrightness,
} kPatternSpiralConfig;

class CPatternSpiral : public IPattern {
private:
	uint8_t changes_ready;
	uint8_t value;
	uint8_t index;

	int pattern[kSunBoardNumberOfLedsPerRay];
	int pattern_next[kSunBoardNumberOfLedsPerRay];
	void set_pattern_hue(int new_hue);
	void set_pattern_brightness(int new_brightness);
	void set_pattern_speed(int new_speed);
	void set_pattern_saturation(int new_saturation);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);

};

extern CPatternSpiral PatternSpiral;

#endif /* PATTERNSpiral_H_ */

/*
 * PatternSpin.h
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#ifndef PATTERNSPIN_H_
#define PATTERNSPIN_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternSpinConfigSetSpeed = 0x01,
	kPatternSpinConfigSetBrightness,
	kPatternSpinConfigSetHue,
	kPatternSpinConfigSetSaturation,
} kPatternSpinConfig;

class CPatternSpin : public IPattern {
private:
	double brightness;
	uint16_t angle;
	uint8_t saturation;
	uint16_t hue;
	uint16_t set_angle;
	uint8_t speed;
	uint8_t value;
	double radians (double d);
	void set_pattern_speed(int new_speed);
	void set_pattern_brightness(int new_brightness);
	void set_pattern_hue(int new_hue);
	void set_pattern_saturation(int new_saturation);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};
extern CPatternSpin PatternSpin;

#endif /* PATTERNSPIN_H_ */

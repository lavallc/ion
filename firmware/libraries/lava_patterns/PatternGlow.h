/*
 * PatternGlow.h
 *
 *  Created on: Feb 18, 2014
 *      Author: salem
 */

#ifndef PATTERNGLOW_H_
#define PATTERNGLOW_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternGlowConfigSetSpeed = 0x01,
	kPatternGlowConfigSetBrightness,
	kPatternGlowConfigSetBrightnessSwing,
	kPatternGlowConfigSetHue,
	kPatternGlowConfigSetSaturation,
} kPatternGlowConfig;

class CPatternGlow : public IPattern {
private:
	double brightness;
	uint8_t swing;
	uint16_t hue;
	uint8_t saturation;
	uint16_t angle;
	uint8_t speed;
	uint16_t set_angle;
	uint8_t value = 255;
	double radians (double d);
	void set_pattern_speed(int new_speed);
	void set_pattern_hue(int new_hue);
	void set_pattern_brightness(int new_brightness);
	void set_pattern_brightness_swing(int new_brightness_swing);
	void set_pattern_saturation(int new_saturation);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};
extern CPatternGlow PatternGlow;

#endif /* PATTERNGLOW_H_ */

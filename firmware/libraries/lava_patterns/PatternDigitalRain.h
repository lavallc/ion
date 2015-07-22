/*
 * PatternDigitalRain.h
 *
 *  Created on: Jan 24, 2014
 *      Author: salem
 */

#ifndef PATTERNDIGITALRAIN_H_
#define PATTERNDIGITALRAIN_H_
#include <cstdint>
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternDigitalRainConfigSetHue = 0x01,
	kPatternDigitalRainConfigSetSaturation,
	kPatternDigitalRainConfigSetSpeed,
} kPatternDigitalRainConfig;

class CPatternDigitalRain : public IPattern {
private:
	int8_t delay = 0;	//This pattern uses the delay method rather than tweening.
	//Arrays are length of 14 to allow the rain to start "above/off" the ray and fall into it
	uint8_t ray1[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t ray2[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t ray3[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8_t ray4[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	uint8_t changes_ready = 0;
	void set_pattern_hue(int new_hue);
	void set_pattern_saturation(int new_saturation);
	void set_pattern_speed(int new_speed);
public:

    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    void get_pattern_config(LumenMoodConfig * configs);
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);

};

extern CPatternDigitalRain PatternDigitalRain;

#endif /* PATTERNDIGITALRAIN_H_ */

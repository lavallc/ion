/*
 * PatternSparkle.h
 *
 *  Created on: Feb 7, 2014
 *      Author: salem
 */

#ifndef PATTERNSPARKLE_H_
#define PATTERNSPARKLE_H_
#include "IPattern.h"
#include "Lumen.h"


typedef enum {
	kPatternSparkleConfigSetHue = 0x01,
	kPatternSparkleConfigSetSaturation,
	kPatternSparkleConfigSetSpeed,
	kPatternSparkleConfigSetBrightness,
} kPatternSparkleConfig;


class CPatternSparkle : public IPattern {
private:
	uint8_t delay;
	uint8_t position;
	uint8_t changes_ready;
	uint8_t rayvals[20];
	uint8_t ledvals[20];
	uint8_t progress[20];
	int16_t power;
	uint8_t brightness;
	void set_pattern_brightness(int new_power);
	void set_pattern_hue(int new_hue);
	void set_pattern_speed(int new_speed);
	void set_pattern_saturation(int new_saturation);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);

};

extern CPatternSparkle PatternSparkle;

#endif /* PATTERNSPARKLE_H_ */

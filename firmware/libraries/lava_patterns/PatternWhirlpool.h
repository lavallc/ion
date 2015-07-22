/*
 * PatternWhirlpool.h
 *
 *  Created on: Feb 20, 2014
 *      Author: salem
 */

#ifndef PATTERNWHIRLPOOL_H_
#define PATTERNWHIRLPOOL_H_
#include "IPattern.h"
#include "Lumen.h"


typedef enum {
	kPatternWhirlpoolConfigSetSpeed = 0x01,
	kPatternWhirlpoolConfigSetHue,
	kPatternWhirlpoolConfigSetSaturation,
	kPatternWhirlpoolConfigSetBassBoost,
} kPatternWhirlpoolConfig;


class CPatternWhirlpool : public IPattern {
private:
	uint8_t index;
	//properties
	uint16_t hue_frequencies[7];
	int8_t brightness_peak =1;
	uint8_t static_speed;
	uint16_t new_hue;
	uint16_t whirlpool_saturation;
	uint8_t changes_ready = 0;
	uint8_t new_saturation;
	int8_t delay = 0;
	int hue = 0;
	int lasthue = 0;
	bool kick_detected = false;
	void set_pattern_hue(int hue_value);
	void set_pattern_saturation(int sat_value);
	void set_pattern_bass_boost(bool new_bass_bost);
	void spread(uint8_t ray, uint8_t led);
	void set_pattern_speed(int new_speed);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};

extern CPatternWhirlpool PatternWhirlpool;

#endif /* PATTERNWHIRLPOOL_H_ */

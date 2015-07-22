/*
 * PatternRave.h
 *
 *  Created on: Jan 26, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNRAVE_H_
#define PATTERNRAVE_H_
#include "IPattern.h"
#include "Lumen.h"
#include "LumenAudio.h"
typedef enum {
	kPatternRaveConfigSetSpeed = 0x01,
	kPatternRaveConfigSetHue,
	kPatternRaveConfigSetSaturation,
	kPatternRaveConfigSetBassBoost,S
} kPatternRaveConfig;
class CPatternRave : public IPattern {
private:
	int8_t brightness_peak =20;
	int8_t last_brightness_peak = 20;
	int8_t delay = 0;
	int hue = 0;
	int lasthue = 0;
	uint8_t changes_ready;
	int8_t bucket_chosen[7] = {0,0,0,0,0,0,0};
	void set_pattern_speed(int new_speed);
	void set_pattern_saturation(int sat_value);
	void set_pattern_party(bool is_party_mode);
	void set_pattern_hue(int hue_value);
	void set_pattern_bass_boost(bool new_bass_boost);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void get_pattern_config(LumenMoodConfig * configs);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternRave PatternRave;

#endif /* PATTERNRAVE_H_ */

/*
 * PatternPulse.h
 *
 *  Created on: Feb 20, 2014
 *      Author: salem
 */

#ifndef PATTERNPULSE_H_
#define PATTERNPULSE_H_
#include "IPattern.h"
#include "LumenAudio.h"
#include "Lumen.h"
#include "PatternHelpers.h"

typedef enum {
	kPatternPulseConfigSetSpeed = 1,
	kPatternPulseConfigSetSoundEnabled,
} kPatternPulseConfig;


class CPatternPulse : public IPattern {
private:
	uint8_t index;
	uint16_t hue = 0;
	uint16_t frequency_hues[7];
	bool sound_enabled;
	void set_pattern_speed(int new_speed);
	void set_pattern_sound(bool is_sound_enabled);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);

    void get_pattern_config(LumenMoodConfig * configs);

};

extern CPatternPulse PatternPulse;

#endif /* PATTERNPULSE_H_ */

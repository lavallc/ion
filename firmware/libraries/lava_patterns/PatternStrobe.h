/*
 * PatternStrobe.h
 *
 *  Created on: Jan 31, 2014
 *      Author: barch
 */

#ifndef PATTERNSTROBE_H_
#define PATTERNSTROBE_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternStrobeConfigSetSpeed = 0x01,
	kPatternStrobeConfigSetHue,
	kPatternStrobeConfigSetSaturation,
	kPatternStrobeConfigSetColorEnabled
} kPatternStrobeConfig;

class CPatternStrobe : public IPattern {
private:
	uint8_t stay_on;
	uint8_t stay_off;
	bool light_on;
	uint8_t count;
	uint8_t changes_ready;
	bool color_enabled;

	void set_pattern_speed(int new_speed);
	void set_pattern_hue(int new_hue);
	void set_pattern_color_enabled(bool enabled);
	void set_pattern_saturation(int new_saturation);
	void toggle();
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternStrobe PatternStrobe;
#endif /* PATTERNSTROBE_H_ */

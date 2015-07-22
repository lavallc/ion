/*
 * PatternLines.h
 *
 *  Created on: Feb 3, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNLINES_H_
#define PATTERNLINES_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternLinesConfigSetHue = 0x01,
	kPatternLinesConfigSetSaturation,
	kPatternLinesConfigSetSpeed,
} kPatternLinesConfig;

class CPatternLines : public IPattern {
private:
	uint8_t changes_ready;
	void set_pattern_hue(int new_hue);
	void set_pattern_saturation(int new_saturation);
	void set_pattern_speed(int new_speed);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};
extern CPatternLines PatternLines;

#endif /* PATTERNLINES_H_ */

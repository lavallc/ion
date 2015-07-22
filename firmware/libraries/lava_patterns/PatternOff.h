/*
 * PatternOff.h
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#ifndef PATTERNOFF_H_
#define PATTERNOFF_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternOffLowGlow = 0x01,

} kPatternOffConfig;

class CPatternOff : public IPattern {
private:
	bool already_set;
	uint8_t last_value, value;
	void set_pattern_lowglow(bool enabled);
	void transition();
	bool finished = true;
	bool low_glow_enabled;
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};

extern CPatternOff PatternOff;
#endif /* PATTERNOFF_H_ */

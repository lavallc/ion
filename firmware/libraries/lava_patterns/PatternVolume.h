/*
 * PatternVolume.h
 *
 *  Created on: March 31, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNVOLUME_H_
#define PATTERNVOLUME_H_
#include "IPattern.h"
#include "Lumen.h"


class CPatternVolume : public IPattern {
private:
		int8_t delay = 0;
		uint8_t bucket = 8;
		uint16_t sum;
		uint16_t sensitivity;
		uint16_t brightness;
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternVolume PatternVolume;

#endif /* PATTERNVOLUME_H_ */

/*
 * PatternBinaryClock.h
 *
 *  Created on: Feb 13, 2014
 *      Author: barch
 */

#ifndef PATTERNBINARYCLOCK_H_
#define PATTERNBINARYCLOCK_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternBinaryClockConfigSetHue = 0x01
} kPatternBinaryClockConfig;

class CPatternBinaryClock : public IPattern {
private:
	uint16_t hour_hue = 0;
	uint16_t minute_hue = 120;
	uint16_t second_hue = 240;

	void set_pattern_hue(uint16_t hr_hue, uint16_t min_hue, uint16_t sec_hue);
	void display_binary_strip(unsigned char ray, unsigned char value, int hue);
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};
extern CPatternBinaryClock PatternBinaryClock;

#endif /* PATTERNBINARYLCOK_H_ */

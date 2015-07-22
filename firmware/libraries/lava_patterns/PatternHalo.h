/*
 * PatternHalo.h
 *
 *  Created on: Jan 30, 2014
 *      Author: billy
 */

#ifndef PATTERNHALO_H_
#define PATTERNHALO_H_
#include "IPattern.h"
#include "Lumen.h"


typedef enum {
	kPatternHaloConfigSetHue = 0x01,
	kPatternHaloConfigSetSpeed,
} kPatternHaloConfig;


class CPatternHalo : public IPattern {
private:
	uint8_t index;
	int8_t direction;
	uint8_t speed;
	uint8_t saturation;
	uint8_t value;
	uint16_t progress;
	uint8_t values[10];
	uint8_t values_next[10];
	//properties
	uint16_t hue;
	void set_pattern_hue(uint16_t new_hue);
	void set_pattern_speed(uint16_t new_speed);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);


};

extern CPatternHalo PatternHalo;

#endif /* PATTERNHALO_H_ */

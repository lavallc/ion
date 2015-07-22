/*
 * PatternIon.h
 *
 *  Created on: Feb 18, 2014
 *      Author: salem
 */

#ifndef PATTERNION_H_
#define PATTERNION_H_
#include "IPattern.h"
#include "Lumen.h"


typedef enum {
	kPatternIonConfigSetHue = 0x01,
	kPatternIonConfigSetSpeed,
} kPatternIonConfig;


class CPatternIon : public IPattern {
private:
	uint8_t index[4];
	uint8_t progress[4];
	int8_t direction[4];

	//properties
	uint8_t speed[4];
	uint16_t hue;
	void set_pattern_hue(int new_hue);
	void set_pattern_speed(int new_speed);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);


};

extern CPatternIon PatternIon;

#endif /* PATTERNION_H_ */

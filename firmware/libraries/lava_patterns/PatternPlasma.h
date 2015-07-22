/*
 * PatternPlasma.h
 *
 *  Created on: Feb 13, 2014
 *      Author: salem
 */

#ifndef PATTERNPLASMA_H_
#define PATTERNPLASMA_H_
#include "IPattern.h"
#include "Lumen.h"
#include "LumenAudio.h"

typedef enum {
	kPatternPlasmaConfigSetHue = 0x01,
	kPatternPlasmaConfigSetSaturation,
	kPatternPlasmaConfigSetSpeed,
	kPatternPlasmaConfigSetSoundEnabled,
} kPatternPlasmaConfig;


class CPatternPlasma : public IPattern {
private:
	bool sound_enabled = true;
	uint8_t changes_ready;
	uint8_t static_speed;
	// hold the state of every LED (0 = base state, 127 = Plasma state, 255 = base state)
	uint8_t pixel_states[40];
	void set_pattern_hue(int new_hue);
	void set_pattern_speed(int new_speed);
	void set_pattern_sound(bool is_sound_enabled);
	void set_pattern_saturation(int new_saturation);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);

    void get_pattern_config(LumenMoodConfig * configs);

};

extern CPatternPlasma PatternPlasma;

#endif /* PATTERNPLASMA_H_ */

/*
 * PatternFireworks.h
 *
 *  Created on: Feb 12, 2014
 *      Author: salem
 */

#ifndef PATTERNFIREWORKS_H_
#define PATTERNFIREWORKS_H_
#include "IPattern.h"
#include "Lumen.h"


typedef enum {
	kPatternFireworksConfigSetHue = 0x01,
	kPatternFireworksConfigSetSaturation,
	kPatternFireworksConfigEnableHue,
	kPatternFireworksConfigSetSpeed,
	kPatternFireworksConfigSetPower,
	kPatternFireworksConfigSetSoundEnabled,
} kPatternFireworksConfig;




class CPatternFireworks : public IPattern {
private:

	typedef enum {
		kFireworksStateFlamingBalls= 0x01,
		kFireworksStateShooting,
		kFireworksStateReporting,
	} kPatternFireworksState;

	typedef enum {
		kFireworksTypeExplosion= 0x01,
		kFireworksTypeAssorted,
		kFireworksTypeLines,
	} kPatternFireworksType;
	uint8_t value[3];
	uint8_t index[3];
	uint8_t saturation[3];
	uint8_t pattern_state[3];
	uint8_t pattern_type[3];;
	uint8_t ray_num[3];
	uint16_t progress[3];
	bool sound_enabled;
	uint16_t speed[3];
	int16_t power, saved_power;
	bool hue_enabled;
	uint8_t changes_ready = 0;
	//properties
	uint16_t temp_speed[3];
	uint16_t hue[3] = {0,0,0};
	void set_pattern_sound(bool is_sound_enabled);
	void set_pattern_enable_hue(bool is_hue_enabled);
	void set_pattern_power(int new_power);
	void reporting(uint8_t firework_number);
	void flaming_balls(uint8_t firework_number);
	void reset(uint8_t firework_number);
	void draw_fireworks(uint8_t firework_number);
	void shooting(uint8_t firework_number);
	void set_pattern_hue(int new_hue);
	void set_pattern_speed(int new_speed);
	void set_pattern_saturation(int new_saturation);
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternFireworks PatternFireworks;

#endif /* PATTERNFIREWORKS_H_ */

/*
 * PatternRaw.h
 *
 *  Created on: Jan 26, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNRAW_H_
#define PATTERNRAW_H_
#include "IPattern.h"
#include "Lumen.h"
#include "LumenAudio.h"
typedef enum {
	kPatternRawConfigBankID = 0x01,
	kPatternRawConfigBankVals,
	kPatternRawConfigLEDID,
	kPatternRawConfigLEDVals,
	kPatternRawConfigFill,
	kPatternRawConfigClear,
	kPatternRawConfigRefresh,

} kPatternRawConfig;
class CPatternRaw : public IPattern {
private:
	bool trigger_refresh = false;
	bool live_update = false;
	bool led_id_set  = false;
	bool bank_id_set = false;
	uint8_t led_id;
	uint8_t bank_id;
	void set_refresh();
	void set_clear();
	void set_fill(RGB * color_vals);
	void set_led_vals(RGB * color_vals);
	void set_led_id(int new_led_id);
	void set_bank_vals(RGB * color_vals);
	void set_bank_id(int new_bank_id);
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternRaw PatternRaw;

#endif /* PATTERNRAW_H_ */

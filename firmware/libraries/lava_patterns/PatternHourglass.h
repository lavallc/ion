/*
 * PatternLines.h
 *
 *  Created on: Feb 6, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNHOURGLASS_H_
#define PATTERNHOURGLASS_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternHourglassConfigSetTime = 0x01,
	kPatternHourglassConfigSetHue,
	kPatternHourglassConfigSetSaturation,
	kPatternHourglassConfigSetRunning,
} kPatternHourglassConfig;

class CPatternHourglass : public IPattern {
private:
	uint8_t grain = 0;

	unsigned int timer_start;	// when the timer was started (seconds uptime)
	unsigned int timer_end;	// when the timer should stop (seconds uptime)
	int last_second;			// used to reset the tick counter every second
	unsigned int seconds_left;
	int total_seconds;
	int amount_bottom;
	int amount_top;
	bool is_running;
	uint8_t changes_ready;
	uint8_t tick_counter = 0;

	void set_pattern_time(int seconds);
	void set_pattern_hue(int new_hue);
	void set_pattern_saturation(int new_saturation);
	void set_pattern_running(bool running);
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};
extern CPatternHourglass PatternHourglass;

#endif /* PATTERNHOURGLASS_H_ */

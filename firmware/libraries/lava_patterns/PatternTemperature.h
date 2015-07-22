/*
 * PatternTemperature.h
 *
 *  Created on: Aulocogust 14, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNTEMPERATURE_H_
#define PATTERNTEMPERATURE_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternTemperatureConfigForecast = 0x01,

} kPatternTemperatureConfig;

class CPatternTemperature : public IPattern {
private:
	void set_pattern_forecast(bool forecast);
	bool forecast;
	int8_t temperature = 0;
	uint16_t sensitivity;
public:
	void get_pattern_config(LumenMoodConfig * configs);
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
};

extern CPatternTemperature PatternTemperature;

#endif /* PATTERNTEMPERATURE_H_ */

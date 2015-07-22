/*
 * PatternWeather.h
 *
 *  Created on: Jan 26, 2014
 *      Author: Salemancer
 */

#ifndef PATTERNWEATHER_H_
#define PATTERNWEATHER_H_
#include "IPattern.h"
#include "Lumen.h"
#include "LumenAudio.h"



typedef enum {
	kPatternWeatherConfigForecast = 0x01

} kPatternWeatherConfig;

class CPatternWeather : public IPattern {
private:
	unsigned int stepvariable;
	unsigned int stepvariable_decimal;
	unsigned int decimal_percent = 0;
	unsigned int base_time = 0;
	int16_t percent = 0;
	unsigned int last_uptime_update;
	typedef enum {
		kWeatherNoData = 0,
		kWeatherClear,
		kWeatherClouds,
		kWeatherRain,
		kWeatherSnow,
		kWeatherThunderstorm,
		kWeatherHazy,
	} kPatternWeatherTypes;
	const uint8_t sun_hue[3] = {60,60,60};
	const uint8_t sun_sat[3] = {127,255,127};
	const uint8_t sun_value[3] = {127,255,127};
	const uint8_t moon_hue[3] = {240,240,240};
	const uint8_t moon_sat[3] = {75,0,75};
	const uint8_t moon_value[3] = {25,125,25};
	uint8_t weather_data = kWeatherClear; //0 = sun, 1 = rain, 2 = snow
	int8_t direction;
	int8_t index;
	bool show_forecast = false;
	bool is_day = true;
	uint8_t lightning[10];
	bool weather_type_now_precipitation = false;
	uint8_t current_step = 0;
	unsigned int current_second= 0;
	unsigned int current_percent = 0;
	unsigned int end_of_day;
	uint16_t hue;
	uint8_t saturation;
	uint8_t value;
	void reset();
	int get_brightness_value();
	void lerp_weather();
	void set_invalid_data();
	void prepare_ground();
	void initialize_clouds();
	void initialize_haze();
	void initialize_pattern();
	void set_weather_type(int new_weather_data);
	void set_percent_cycle(int new_percent);
	void prepare_precipitation();
	void prepare_sun_moon();
	void precipitate(int sat);
	void prepare_day_night();
	void weather_snow();
	void weather_rain();
	void update_weather();
	void weather_clouds();
	void weather_clear();
	void weather_haze();
	void set_weather_forecast(bool forecast);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};

extern CPatternWeather PatternWeather;

#endif /* PATTERNWEATHER_H_ */

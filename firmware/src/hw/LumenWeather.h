/*
 * LumenWeather.h
 *
 *  Created on: Jun 12, 2014
 *      Author: ebarch
 */

#ifndef LUMENWEATHER_H_
#define LUMENWEATHER_H_

extern "C" {
#include "nrf.h"
}
class CLumenWeather {
public:
	int8_t get_current_temp();
	uint8_t get_current_conditions();
	int8_t get_forecast_temp();
	uint8_t get_forecast_conditions();

	uint8_t get_sunrise_hr();
	uint8_t get_sunrise_min();
	uint8_t get_sunset_hr();
	uint8_t get_sunset_min();
	unsigned int _last_uptime_update = 0;
	bool is_valid();
	void set_weather(int8_t current_temp, uint8_t current_conditions, int8_t forecast_temp, uint8_t forecast_conditions);
	void set_sun_times(uint8_t sunrise_24hr, uint8_t sunrise_min, uint8_t sunset_24hr, uint8_t sunset_min);
private:
	int8_t _current_temp = 75;
	uint8_t _current_conditions = 1;
	int8_t _forecast_temp = 75;
	uint8_t _forecast_conditions = 1;

	uint8_t _sunrise_24hr = 0;
	uint8_t _sunrise_min = 0;
	uint8_t _sunset_24hr = 12;
	uint8_t _sunset_min = 0;


	bool _has_been_set = false;
};

extern CLumenWeather LumenWeather;

#endif /* LUMENWEATHER_H_ */

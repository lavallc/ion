/*
 * LumenWeather.cpp
 *
 *  Created on: Jun 12, 2014
 *      Author: ebarch
 */

#include "LumenWeather.h"
#include "LumenClock.h"
CLumenWeather LumenWeather;


void CLumenWeather::set_weather(int8_t current_temp, uint8_t current_conditions, int8_t forecast_temp, uint8_t forecast_conditions) {
	_current_temp = current_temp;
	_current_conditions = current_conditions;
	_forecast_temp = forecast_temp;
	_forecast_conditions = forecast_conditions;
	_last_uptime_update = LumenClock.get_uptime();
	_has_been_set = true;
}

void CLumenWeather::set_sun_times(uint8_t sunrise_24hr, uint8_t sunrise_min, uint8_t sunset_24hr, uint8_t sunset_min) {
	_sunrise_24hr = sunrise_24hr;
	_sunrise_min = sunrise_min;
	_sunset_24hr = sunset_24hr;
	_sunset_min = sunset_min;
}

uint8_t CLumenWeather::get_sunrise_hr() {
	return _sunrise_24hr;

}

uint8_t CLumenWeather::get_sunrise_min() {
	return _sunrise_min;
}

uint8_t CLumenWeather::get_sunset_hr() {
	return _sunset_24hr;
}

uint8_t CLumenWeather::get_sunset_min() {
	return _sunset_min;
}

int8_t CLumenWeather::get_current_temp() {
	return _current_temp;
}

uint8_t CLumenWeather::get_current_conditions() {
	return _current_conditions;
}

int8_t CLumenWeather::get_forecast_temp() {
	return _forecast_temp;
}

uint8_t CLumenWeather::get_forecast_conditions() {
	return _forecast_conditions;
}

bool CLumenWeather::is_valid() {
	// we must have been updated in the last two hours for this to be valid
	return (_has_been_set && ((LumenClock.get_uptime() - _last_uptime_update) < (60 * 120)));
}

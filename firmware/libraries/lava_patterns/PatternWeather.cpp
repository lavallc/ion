
/*
 * PatternWeather.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: Salemancer
 *      Pattern Creator: Salemancer
 */

#include "PatternWeather.h"
#include "PatternHelpers.h"
#include "LumenClock.h"
#include "LumenWeather.h"
#include "Serial.h"
#include <cmath>

CPatternWeather PatternWeather;
#define kLengthOfDayInMinutes 1440
#define kSecondsToSteps 33
#define kMinutesToSteps (33 * 60)

#define kPercentIncrementsPerDay 5100 // = 20 LED movements * 255 progress per LED

void CPatternWeather::start(LumenMoodConfig * configs, bool restore_to_defaults) {

	weather_data = kWeatherNoData;
	if (configs == NULL) {
		show_forecast = false;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		show_forecast = (bool)configs->config_data[0].data;
	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
	PatternHelpers.erase_all();
	end_of_day = 5;
	this->initialize_pattern();
}

void CPatternWeather::get_pattern_config(LumenMoodConfig * configs) {
	configs->config_data[0].config_id = kPatternWeatherConfigForecast;
	configs->config_data[0].data = (uint8_t)show_forecast;


}
void CPatternWeather::initialize_pattern() {

	current_second = (int)LumenClock.get_hours_24() * 3600 + (int)LumenClock.get_minutes() * 60 + (int)LumenClock.get_seconds();
	Serial.debug_print(current_second);
	this->last_uptime_update = LumenWeather._last_uptime_update;
	if (show_forecast)
		current_second = (current_second + 21600) % 86400;
	base_time = LumenClock.get_uptime();
	Serial.debug_print(current_second);
	int sunrise_second = (int)LumenWeather.get_sunrise_hr() * 3600 + (int)LumenWeather.get_sunrise_min() * 60;
	int sunset_second = (int)LumenWeather.get_sunset_hr() * 3600 + (int)LumenWeather.get_sunset_min() * 60;
	end_of_day = abs(sunset_second - sunrise_second) / 60;
	Serial.debug_print(end_of_day);
	if (current_second < sunset_second && current_second > sunrise_second) {
		is_day = true;
		current_second = current_second - sunrise_second;
		Serial.debug_print("mid");
	}
	else if (current_second < sunrise_second) {
		is_day = false;
		end_of_day = kLengthOfDayInMinutes - end_of_day;
		current_second = current_second + (kLengthOfDayInMinutes * 60 - sunset_second);
		Serial.debug_print("morn");
	}
	else {
		is_day = false;
		end_of_day = kLengthOfDayInMinutes - end_of_day;
		current_second = current_second - sunset_second;
		Serial.debug_print("night");
	}
	Serial.debug_print(current_second);
	//This stepvariable may not be needed... Also, it could be overflowing.
	stepvariable = (kMinutesToSteps * end_of_day) / 5100;//(end_of_day * kMinutesToSteps) / kPercentIncrementsPerDay;
	stepvariable_decimal = ((end_of_day * kMinutesToSteps * 100) / 5100) - (stepvariable * 100);//((end_of_day * kMinutesToSteps * 100) / kPercentIncrementsPerDay) - (stepvariable * 100);
	stepvariable_decimal = 100 * stepvariable / stepvariable_decimal;
	stepvariable_decimal += 2;

	current_percent = ((current_second) * 5100) / (end_of_day *  60);
	Serial.debug_print(current_percent);
	percent = current_percent % 255;

	//Get info for current pattern state
	if (current_percent / 255 >=10) {
		direction = 1;
		index = (current_percent - (255 * 10)) / 255;
	}
	else {
		direction = -1;
		index = 10 - (current_percent / 255);
	}
	Serial.debug_print(index);
	this->prepare_day_night();
	this->prepare_sun_moon();
	this->prepare_ground();
	for(int i=0; i<9; i++) {
		PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
		PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
		PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
	}

	//Get info for pattern_next state
	if (current_percent / 255 >= 10) {
		direction = 1;
		index = (current_percent - (255 * 9)) / 255;
	}
	else {
		direction = -1;
		index = 9 - (current_percent / 255);
	}
	this->prepare_day_night();
	this->prepare_sun_moon();
	this->prepare_ground();

	PatternHelpers.hue[0][9] = 120;
	PatternHelpers.saturation[0][9] = 255;
	PatternHelpers.value[0][9] = this->get_brightness_value();
	Serial.debug_print("V");
	Serial.debug_print(stepvariable);
	Serial.debug_print(stepvariable_decimal);
}

void CPatternWeather::initialize_clouds() {

	this->prepare_day_night();
	for(int i = 0; i < 3; i++) {
		PatternHelpers.value_next[0][i] = this->get_brightness_value();
		PatternHelpers.saturation_next[0][i] = 0;

	}
	this->prepare_ground();
	for(int i=0; i<10; i++) {
		PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
		PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
		PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
	}
}

void CPatternWeather::initialize_haze() {

	this->prepare_day_night();
	for(int i = 5; i < 8; i++) {
		PatternHelpers.value_next[0][i] = this->get_brightness_value();
		PatternHelpers.saturation_next[0][i] = 50;

	}
	PatternHelpers.value_next[0][4] = this->get_brightness_value();
	PatternHelpers.saturation_next[0][4] = 50;
	PatternHelpers.value_next[0][8] = this->get_brightness_value();
	PatternHelpers.saturation_next[0][8] = 200;
	this->prepare_ground();
	for(int i=0; i<10; i++) {
		PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
		PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
		PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
	}
}


void CPatternWeather::reset() {
	current_percent = 0;
	current_second = 0;
	current_step = 0;
	//See explanation down in PatternStep
	stepvariable = (21 * 60 * 5) / 5100;//(end_of_day * kMinutesToSteps) / kPercentIncrementsPerDay;
	stepvariable_decimal = ((5 * 21 * 60 * 100) / 5100) - (stepvariable * 100);//((end_of_day * kMinutesToSteps * 100) / kPercentIncrementsPerDay) - (stepvariable * 100);
	stepvariable_decimal = 100 * stepvariable / stepvariable_decimal;
	stepvariable_decimal += 2;
	decimal_percent = 0;
	percent = 0;
	index = 9;
	//this->prepare_day_night();
	//this->prepare_sun_moon();
	//this->prepare_ground();
	Serial.debug_print("V");
	Serial.debug_print(stepvariable);
	Serial.debug_print(stepvariable_decimal);
}

void CPatternWeather::set_invalid_data(){
	Lumen.leds->set_level_hsv(0, 240, 255, 30);
	Lumen.leds->set_level_hsv(1, 240, 255, 30);
	Lumen.leds->set_level_hsv(2, 240, 255, 30);
	Lumen.leds->set_level_hsv(3, 240, 255, 30);
	Lumen.leds->set_level_hsv(4, 240, 255, 30);
	Lumen.leds->set_level_hsv(5, 240, 255, 30);
	Lumen.leds->set_level_hsv(6, 240, 255, 30);
	Lumen.leds->set_level_hsv(7, 240, 255, 30);
	Lumen.leds->set_level_hsv(8, 240, 255, 30);
	Lumen.leds->set_level_hsv(9, 120, 255, 30);

}

void CPatternWeather::prepare_sun_moon() {
	if (index != 10) {
		if (is_day) {	//---//DAY//---//
			if (index != 0) {
				PatternHelpers.hue_next[0][index-1] = sun_hue[0];
				PatternHelpers.saturation_next[0][index-1] = sun_sat[0];
				PatternHelpers.value_next[0][index-1] = sun_value[0];
			}
			PatternHelpers.hue_next[0][index] = sun_hue[1];
			PatternHelpers.saturation_next[0][index] = sun_sat[1];
			PatternHelpers.value_next[0][index] = sun_value[1];
			if (index != 9) {
				PatternHelpers.hue_next[0][index+1] = sun_hue[2];
				PatternHelpers.saturation_next[0][index+1] = sun_sat[2];
				PatternHelpers.value_next[0][index+1] = sun_value[2];
			}
			//Orange sun at sunset/sunrise.
			if (index == 8) {
				PatternHelpers.hue_next[0][6] = 40;
				PatternHelpers.saturation_next[0][6] = 230;
				PatternHelpers.value_next[0][6] = 102;
				PatternHelpers.hue_next[0][7] = 35;
				PatternHelpers.saturation_next[0][7] = 230;
				PatternHelpers.value_next[0][7] = 153;
				PatternHelpers.hue_next[0][8] = 30;
				PatternHelpers.saturation_next[0][8] = 230;
				PatternHelpers.value_next[0][8] = 255;
			}
		}
		else {	//---//NIGHT//---//
			if (index != 0) {
				PatternHelpers.hue_next[0][index-1] = moon_hue[0];
				PatternHelpers.saturation_next[0][index-1] = moon_sat[0];
				PatternHelpers.value_next[0][index-1] = moon_value[0];
			}
			PatternHelpers.hue_next[0][index] = moon_hue[1];
			PatternHelpers.saturation_next[0][index] = moon_sat[1];
			PatternHelpers.value_next[0][index] = moon_value[1];
			if (index != 9) {
				PatternHelpers.hue_next[0][index+1] = moon_hue[2];
				PatternHelpers.saturation_next[0][index+1] = moon_sat[2];
				PatternHelpers.value_next[0][index+1] = moon_value[2];
			}
		}
	}
}

void CPatternWeather::weather_clear() {
	if(percent >= 255 ){//&& current_percent < 4500) {
		index += 1*direction;
		//At the top, turn around.
		if(index < 0) {
			direction = 1;
			index = 1;
		}
		//When we reach the bottom, we need to wait for the timer to trigger a new day.
		//So, if we aren't at the bottom, keep up the procedure below.
		if (index <= 10) {
			percent = 0;

			//lets copy pattern_next into pattern
			for(int i=0; i<10; i++) {
				PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
				PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
				PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
			}
		}
		else {
			index = 10;
		}
		this->prepare_day_night();
		this->prepare_sun_moon();
		this->prepare_ground();
	}
	this->lerp_weather();

}

void CPatternWeather::prepare_ground() {
	PatternHelpers.hue_next[0][9] = 120;
	PatternHelpers.saturation_next[0][9] = 255;
	PatternHelpers.value_next[0][9] = this->get_brightness_value();
}

void CPatternWeather::lerp_weather() {
	for(int i = 0; i<=9; i++) {
			uint16_t h;
			uint16_t s;
			//On the edge of the sun/moon, we need to lerp through the middle of the HSV spectrum, rather than around the color wheel.
			if (abs(PatternHelpers.hue[0][i] - PatternHelpers.hue_next[0][i]) > 60) {
				//Move saturation at double speed to 0, while hue lerps at normal rate
				if (percent <= 127) {
					s = PatternHelpers.lerp(PatternHelpers.saturation[0][i], 0, PatternHelpers.value_cap(percent * 2, 255, 0));
					h = PatternHelpers.hue[0][i];
				}
				//Move saturation at double speed back out to proper saturation, while hue lerps at normal rate.
				else {
					s = PatternHelpers.lerp(0, PatternHelpers.saturation_next[0][i], PatternHelpers.value_cap((percent - 127) * 2, 255, 0));
					h = PatternHelpers.hue_next[0][i];
				}
			}
			else {
				//standard lerping
				h = PatternHelpers.lerp(PatternHelpers.hue[0][i] , PatternHelpers.hue_next[0][i], PatternHelpers.value_cap(percent, 255, 0));
				s = PatternHelpers.lerp(PatternHelpers.saturation[0][i], PatternHelpers.saturation_next[0][i], PatternHelpers.value_cap(percent, 255, 0));
			}

			uint16_t v = PatternHelpers.lerp(PatternHelpers.value[0][i], PatternHelpers.value_next[0][i], PatternHelpers.value_cap(percent, 255, 0));
			//if (LumenClock.has_been_set() && LumenWeather.is_valid())
			Lumen.leds->set_level_hsv(i, h, s, v);
	}

}

int CPatternWeather::get_brightness_value() {
	//Brightness is determined by how far into day(or night) we are.
	if (is_day) {
		return 200;
		//if (current_percent > 500)
		//	return PatternHelpers.value_cap((1000 - current_percent) / 2, 204, 55);
		//else
		//	return PatternHelpers.value_cap(current_percent / 2, 204, 50);
	}
	else {
		return 125;
		//if (current_percent > 500)
		//	return PatternHelpers.value_cap((1000 - current_percent) / 4, 125, 62);
		//else
		//	return PatternHelpers.value_cap(current_percent / 4, 75, 25);
	}

}
void CPatternWeather::weather_clouds() {
	if(percent >= 255 ){
		index++;
		percent = 0;
		for(int i = 0; i<10; i++) {
			PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
			PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
			PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
		}
		this->prepare_day_night();
		for(int i = 0; i < 3; i++) {
			PatternHelpers.value_next[0][i] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][i] = 0;
		}
		if (index % 2 == 0) {
			PatternHelpers.value_next[0][3] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][3] = 0;

		}

		this->prepare_ground();
	}

	this->lerp_weather();
}

void CPatternWeather::weather_haze() {
	if(percent >= 255 ){
		index++;
		percent = 0;
		for(int i = 0; i<10; i++) {
			PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
			PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
			PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
		}
		this->prepare_day_night();
		for(int i = 5; i < 8; i++) {
			PatternHelpers.value_next[0][i] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][i] = 50;
		}
		if (index % 2 == 0) {
			PatternHelpers.value_next[0][4] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][4] = 50;
			PatternHelpers.value_next[0][8] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][8] = 200;

		}
		else {
			PatternHelpers.value_next[0][4] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][4] = 200;
			PatternHelpers.value_next[0][8] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][8] = 50;
		}

		this->prepare_ground();
	}

	this->lerp_weather();
}

void CPatternWeather::prepare_day_night() {
	//Set background for day or night.
	for(int i=0; i<10; i++) {
		if (is_day) {
			PatternHelpers.value_next[0][i] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][i] = 255;
			PatternHelpers.hue_next[0][i] = 190;
		}
		else {
			PatternHelpers.value_next[0][i] = this->get_brightness_value();
			PatternHelpers.saturation_next[0][i] = 255;
			PatternHelpers.hue_next[0][i] = 240;
		}
	}
}

void CPatternWeather::precipitate(int sat) {
	//Show the rain/snow
	for (int i = 0;i < 10; i++) {
		for (int j = 0; j < 4; j++) {
			if (PatternHelpers.value_next[j][i] > 80) {
				Lumen.leds->set_pixel_hsv(j, i, PatternHelpers.hue_next[j][i], sat, PatternHelpers.value_next[j][i]);
			}
			else {
				Lumen.leds->set_pixel_hsv(j, i, PatternHelpers.hue_next[j][i], 255, PatternHelpers.value_next[j][i]);
			}
		}
	}
	//Clouds
	Lumen.leds->set_level_hsv(0, 120, 0, this->get_brightness_value());

	//Ground
	if (weather_data == kWeatherRain) {
		Lumen.leds->set_level_hsv(9, 120,  255, this->get_brightness_value());
	}
	else {
		Lumen.leds->set_level_hsv(9, 120,  178, this->get_brightness_value());
	}

	//Shift precipitation down
	for (int j = 9; j > 0; j--) {
		PatternHelpers.value_next[0][j] = PatternHelpers.value_next[0][j - 1];
		PatternHelpers.value_next[1][j] = PatternHelpers.value_next[1][j - 1];
		PatternHelpers.value_next[2][j] = PatternHelpers.value_next[2][j - 1];
		PatternHelpers.value_next[3][j] = PatternHelpers.value_next[3][j - 1];
		PatternHelpers.hue_next[0][j] = PatternHelpers.hue_next[0][j - 1];
		PatternHelpers.hue_next[1][j] = PatternHelpers.hue_next[1][j - 1];
		PatternHelpers.hue_next[2][j] = PatternHelpers.hue_next[2][j - 1];
		PatternHelpers.hue_next[3][j] = PatternHelpers.hue_next[3][j - 1];
	}
	//Clear top row.
	PatternHelpers.hue_next[0][0] = 240;
	PatternHelpers.hue_next[1][0] = 240;
	PatternHelpers.hue_next[2][0] = 240;
	PatternHelpers.hue_next[3][0] = 240;
	PatternHelpers.value_next[0][0] = 80;
	PatternHelpers.value_next[1][0] = 80;
	PatternHelpers.value_next[2][0] = 80;
	PatternHelpers.value_next[3][0] = 80;
}

void CPatternWeather::prepare_precipitation() {
	//Randomly choose either rain drops or snow flakes to drop on certain rays.
	if (percent >= 255)
		percent = 0;
	int ray_num = rand() % 4;

	for (int i = ray_num; i < ray_num + 4; i++) {
		int is_drop;
		if (weather_data == kWeatherRain)
			is_drop = (rand() % 3) + 1;
		else
			is_drop = (rand() % 4) + 1;
		if (is_drop > 1)
			is_drop = 0;

		if (PatternHelpers.value_next[i % 4][0] == 80 && PatternHelpers.value_next[i % 4][1] == 80 && PatternHelpers.value_next[i % 4][2] == 80) {
			if (is_drop) {
				if (weather_data == kWeatherRain) {
					PatternHelpers.value_next[i % 4][0] = 160;
					PatternHelpers.hue_next[i % 4][0] = 190;
				}
				PatternHelpers.value_next[i % 4][1] = 200;
				PatternHelpers.hue_next[i % 4][1] = 190;
			}
			else {
				PatternHelpers.value_next[i % 4][0] = 80;
				PatternHelpers.hue_next[i % 4][0] = 240;
				PatternHelpers.value_next[i % 4][1] = 80;
				PatternHelpers.hue_next[i % 4][1] = 240;
			}
		}
	}

}

void CPatternWeather::step() {


	//This handles precision error when calculating how often to increment the percent.
	//A day is separated into 4590 percent increments. That is 18 LED movements (sun up and back down)and 255 percent per movement.
	//We take the "end_of_day"(minutes until end of day) and find out how many PatternSteps must be called to increment the percent variable(4590 times).
	//However, it may come out to be like 84.7 steps per increment of percent.
	//This decimal means when the sun is back down, there will be substantial time until the end of day actually happens. That is bad.
	//So we find out how often do we decrement the percent so the 4590 percent total happens about at the end of the day.
	/*decimal_percent++;
	if (decimal_percent == stepvariable_decimal) {

		//Serial.debug_print("-");
		//Serial.debug_print(stepvariable_decimal);
		if (percent > 1){//(decimal_percent / stepvariable_decimal)) {
			decimal_percent = 0;
			percent = percent -1;// = percent - (decimal_percent / stepvariable_decimal);
			current_percent = current_percent -1;
		}
		else
			decimal_percent--;
	}*/

	current_step++;
	unsigned int uptime = LumenClock.get_uptime();
	//Standard timing stuff.
	if (uptime != base_time)  {
		uptime = uptime - base_time;
		base_time += uptime;
		current_second += uptime;
		Serial.debug_print("Weather");
		current_step = 0;
		//current_percent = ((current_second * kSecondsToSteps + current_step) * kPercentIncrementsPerDay ) / (end_of_day * kMinutesToSteps);
		Serial.debug_print(percent);
		Serial.debug_print(current_second);
		Serial.debug_print(weather_data);
		if (current_second  >= end_of_day * 60) {
			for(int i=0; i<10; i++) {
				PatternHelpers.hue[0][i] = PatternHelpers.hue_next[0][i];
				PatternHelpers.saturation[0][i] = PatternHelpers.saturation_next[0][i];
				PatternHelpers.value[0][i] = PatternHelpers.value_next[0][i];
			}
			is_day = !is_day;
			direction *= -1;
			end_of_day = kLengthOfDayInMinutes - end_of_day;
			this->reset();
			index = 9;
			Serial.debug_print("OM");
			Serial.debug_print(stepvariable);

		}
	}

	if ((current_second * kSecondsToSteps + current_step) % stepvariable == 0) {
		percent++;
		current_percent++;
	}

	this->update_weather();
	//Weather!
	switch(weather_data) {
		case kWeatherClear : {
			this->weather_clear();
			break;
		}
		case kWeatherClouds : {
			this->weather_clouds();
			break;
		}
		case kWeatherHazy : {
			this->weather_haze();
			break;
		}
		case kWeatherThunderstorm :
			if (rand() % 200 == 0 && lightning[0] == 0)
				lightning[0] = 4;
		case kWeatherRain : {
			if (current_step % 2 == 0) {
				if (weather_type_now_precipitation) {
					for (int i = 0; i < 10; i++) {
						PatternHelpers.hue_next[0][i] = 240;
						PatternHelpers.hue_next[1][i] = 240;
						PatternHelpers.hue_next[2][i] = 240;
						PatternHelpers.hue_next[3][i] = 240;
						PatternHelpers.value_next[0][i] = 80;
						PatternHelpers.value_next[1][i] = 80;
						PatternHelpers.value_next[2][i] = 80;
						PatternHelpers.value_next[3][i] = 80;
					}
					weather_type_now_precipitation = false;
				}
				this->prepare_precipitation();
				this->precipitate(255);
				if (lightning[0] == 4) {
					uint8_t ray = rand() % 4;
					lightning[1] = ray;
					/*for (int i = 2; i < 8; i++) {
						uint8_t left = rand() % 5;
						if (left == 4) {
							lightning[i] = (ray + 1) % 4;

						}
						else {
							uint8_t right = rand() % 5;
							if (right == 4) {
								lightning[i] = (ray + 3) % 4;
							}
							else {
								lightning[i] = ray;
							}
						}
					}*/
					lightning[2] = (ray + 1) % 4;
					lightning[3] = ray;
					lightning[4] = (ray + 3) % 4;
					lightning[5] = ray;
					lightning[6] = ray;
					lightning[7] = ray;
					lightning[8] = ray;
				}

				if (lightning[0] > 0) {
					for (int i = 1; i <= 10 - (lightning[0] * 2); i++) {
						Lumen.leds->set_pixel_hsv(lightning[i], i, 60, 255, 255);
					}
					lightning[0] = lightning[0] - 1;
				}
			}
			break;
		}
		case kWeatherSnow : {
			if (current_step % 6 == 0) {
				if (weather_type_now_precipitation) {
					for (int i = 0; i < 10; i++) {
						PatternHelpers.hue_next[0][i] = 240;
						PatternHelpers.hue_next[1][i] = 240;
						PatternHelpers.hue_next[2][i] = 240;
						PatternHelpers.hue_next[3][i] = 240;
						PatternHelpers.value_next[0][i] = 80;
						PatternHelpers.value_next[1][i] = 80;
						PatternHelpers.value_next[2][i] = 80;
						PatternHelpers.value_next[3][i] = 80;
					}
					weather_type_now_precipitation = false;
				}
				this->prepare_precipitation();
				this->precipitate(0);
			}
			break;
		}
	}
	if (!LumenClock.has_been_set() || !LumenWeather.is_valid())
		this->set_invalid_data();
	if (LumenWeather._last_uptime_update > this->last_uptime_update) {
		weather_data = kWeatherNoData;
		this->initialize_pattern();
	}
}


void CPatternWeather::update_weather() {
	uint8_t conditions;
	if (show_forecast) {
		conditions = LumenWeather.get_forecast_conditions();
	}
	else {
		conditions = LumenWeather.get_current_conditions();
	}
	this->set_weather_type(conditions);
}

void CPatternWeather::set_weather_type(int new_weather_data) {
	if (new_weather_data == kWeatherNoData)
		new_weather_data = kWeatherClear;
	if (weather_data != kWeatherClear && new_weather_data == kWeatherClear) {
		this->initialize_pattern();
	}
	if (weather_data != kWeatherClouds && new_weather_data == kWeatherClouds) {
		this->initialize_clouds();
	}
	if (weather_data != kWeatherRain && new_weather_data == kWeatherRain) {
		lightning[0] = 0;
		this->weather_type_now_precipitation = true;
	}
	if (weather_data != kWeatherSnow && new_weather_data == kWeatherSnow)
		this->weather_type_now_precipitation = true;
	if (weather_data != kWeatherHazy && new_weather_data == kWeatherHazy)
		this->initialize_haze();
	if (weather_data != kWeatherThunderstorm && new_weather_data == kWeatherThunderstorm) {
		lightning[0] = 0;
		this->weather_type_now_precipitation = true;
	}


	weather_data = new_weather_data;
}

void CPatternWeather::set_weather_forecast(bool forecast) {
	show_forecast = forecast;
	weather_data = kWeatherNoData;
	this->initialize_pattern();
}


uint32_t CPatternWeather::get_pattern_id() {
	return 0x00000008;
}

void CPatternWeather::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternWeatherConfigForecast: this->set_weather_forecast((bool)data); break;
		}
}

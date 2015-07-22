
/*
 * PatternVolume.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: Salemancer
 *      Pattern Creator: Salemancer
 */
#include "PatternHelpers.h"
#include "PatternTemperature.h"
#include "LumenWeather.h"
#include <cmath>

CPatternTemperature PatternTemperature;

void CPatternTemperature::start(LumenMoodConfig * configs, bool restore_to_defaults) {

	sensitivity = 10;
	if (configs == NULL) {
		forecast = false;
	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		forecast = configs->config_data[0].data;

	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}

}
void CPatternTemperature::get_pattern_config(LumenMoodConfig * configs){
	configs->config_data[0].config_id = kPatternTemperatureConfigForecast;
	configs->config_data[0].data = forecast;


}

void CPatternTemperature::step() {

	if (LumenWeather.is_valid()) {
		if (forecast)
			temperature = LumenWeather.get_forecast_temp();
		else
			temperature = LumenWeather.get_current_temp();

		LumenLeds.set_level_hsv(0,0,255, temperature >= 100 ? 255 : PatternHelpers.value_cap((temperature - 90), 100, 0) * (255 / 10));
		LumenLeds.set_level_hsv(1,0,255, temperature >= 90 ? 255 : PatternHelpers.value_cap((temperature - 85), 100, 0) * (255 / 5));
		LumenLeds.set_level_hsv(2,30,255, temperature >= 85 ? 255 : PatternHelpers.value_cap((temperature - 80), 100, 0) * (255 / 5));
		LumenLeds.set_level_hsv(3,30,255, temperature >= 80 ? 255 : PatternHelpers.value_cap((temperature - 75), 100, 0) * (255 / 5));
		LumenLeds.set_level_hsv(4,120,255, temperature >= 75 ? 255 : PatternHelpers.value_cap((temperature - 70), 100, 0) * (255 / 5));
		LumenLeds.set_level_hsv(5,120,255, temperature >= 70 ? 255 : PatternHelpers.value_cap((temperature - 60), 100, 0) * (255 / 10));
		LumenLeds.set_level_hsv(6,200,255, temperature >= 60 ? 255 : PatternHelpers.value_cap((temperature - 48), 100, 0) * (255 / 12));
		LumenLeds.set_level_hsv(7,200,255, temperature >= 48 ? 255 : PatternHelpers.value_cap((temperature - 32), 100, 0) * (255 / 16));
		LumenLeds.set_level_hsv(8,240,255, temperature >= 32 ? 255 : temperature * (255 / 32));
		LumenLeds.set_level_hsv(9,240,255, 255);
	}
	else {
		LumenLeds.set_level_hsv(0,0,255, 25);
		LumenLeds.set_level_hsv(1,0,255, 25);
		LumenLeds.set_level_hsv(2,30,255, 25);
		LumenLeds.set_level_hsv(3,30,255, 25);
		LumenLeds.set_level_hsv(4,120,255, 25);
		LumenLeds.set_level_hsv(5,120,255, 25);
		LumenLeds.set_level_hsv(6,200,255, 25);
		LumenLeds.set_level_hsv(7,200,255, 25);
		LumenLeds.set_level_hsv(8,240,255, 25);
		LumenLeds.set_level_hsv(9,240,255, 25);
	}

}

uint32_t CPatternTemperature::get_pattern_id() {
	return 0x0000000F;
}

void CPatternTemperature::set_pattern_forecast(bool is_forecast) {
	forecast = is_forecast;
}

void CPatternTemperature::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
			case kPatternTemperatureConfigForecast: this->set_pattern_forecast((int)data); break;

		}
}

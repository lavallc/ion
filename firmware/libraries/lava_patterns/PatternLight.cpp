/*
 * PatternLight.cpp
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#include "PatternLight.h"
#include "PatternHelpers.h"

CPatternLight PatternLight;

// 1900K
void CPatternLight::setCandle() {
	PatternHelpers.hue_next[0][0] = 14;
	PatternHelpers.saturation_next[0][0] = 242;
}


// 2850K
void CPatternLight::setIncandescent() {
	PatternHelpers.hue_next[0][0] = 31;
	PatternHelpers.saturation_next[0][0] = 98;
}

// 3200K
void CPatternLight::setBlueSky() {
	PatternHelpers.hue_next[0][0] = 190;
	PatternHelpers.saturation_next[0][0] = 200;
}

// ~5000K
void CPatternLight::setFluorescent() {
	PatternHelpers.hue_next[0][0] = 190;
	PatternHelpers.saturation_next[0][0] = 70;
}

// 6000K
void CPatternLight::setSunlight() {
	PatternHelpers.saturation_next[0][0] = 0;
}

void CPatternLight::get_pattern_config(LumenMoodConfig * configs) {
	configs->config_data[0].config_id = kPatternLightConfigSetType;
	configs->config_data[0].data = type;
	configs->config_data[1].config_id = kPatternLightConfigSetBrightness;
	configs->config_data[1].data = PatternHelpers.value_next[0][0];
	configs->config_data[2].config_id = kPatternLightConfigSetHue;
	configs->config_data[2].data = PatternHelpers.hue_next[0][0];
	configs->config_data[3].config_id = kPatternLightConfigSetSaturation;
	configs->config_data[3].data = PatternHelpers.saturation_next[0][0];
	configs->config_data[4].config_id = kPatternLightConfigSetGlow;
	configs->config_data[4].data = low_glow;

}
void CPatternLight::start(LumenMoodConfig * configs, bool restore_to_defaults) {


	PatternHelpers.hue[0][0] = 0;
	PatternHelpers.saturation[0][0] = 0;
	PatternHelpers.value[0][0] = 0;
	finished = false;
	PatternHelpers.progress = 0;
	changes_ready = 0;
	if (configs == NULL) {
		PatternHelpers.value_next[0][0] = 190;
		PatternHelpers.hue_next[0][0] = 0;
		low_glow = false;
		this->set_pattern_type(kPatternLightTypeSunlight);


	}
	else if(configs->mood_id == (uint8_t)get_pattern_id()) {
		PatternHelpers.progress = 255;
		type = configs->config_data[0].data;
		PatternHelpers.value_next[0][0] = configs->config_data[1].data;
		PatternHelpers.hue_next[0][0] = configs->config_data[2].data;
		PatternHelpers.saturation_next[0][0] = configs->config_data[3].data;
		low_glow = configs->config_data[4].data;
		set_pattern_type(type);

	}
	else {
		//We were sent the wrong configs!
		//This will indicate a restart;
	}
}

void CPatternLight::step() {
	if (changes_ready == 3) {
		PatternHelpers.hue_next[0][0] = PatternHelpers.hue[0][1];
		PatternHelpers.saturation_next[0][0] = PatternHelpers.saturation[0][1];
		PatternHelpers.hue[0][0] = h;
		PatternHelpers.saturation[0][0] = s;
		set_pattern_type(kPatternLightTypeColor);
		changes_ready = 0;
	}
	if (!finished) {
		PatternHelpers.progress += 5;
		this->transition();
	}
}

void CPatternLight::transition() {

	if ((PatternHelpers.hue_next[0][0] - PatternHelpers.hue[0][0]) > 180) {
		PatternHelpers.hue[0][0] += 360;
	}
	else if ((PatternHelpers.hue[0][0] - PatternHelpers.hue_next[0][0]) > 180) {
		PatternHelpers.hue_next[0][0] += 360;
	}

	if ((PatternHelpers.hue[0][0] > PatternHelpers.hue_next[0][0]) && ((PatternHelpers.hue[0][0] - PatternHelpers.hue_next[0][0]) >= 120)) {
		if (PatternHelpers.progress < 128) {
			s = PatternHelpers.lerp(PatternHelpers.saturation[0][0], 0, PatternHelpers.progress * 2);
			h = PatternHelpers.hue[0][0] % 360;
		}
		else {
			s = PatternHelpers.lerp(0, PatternHelpers.saturation_next[0][0], (PatternHelpers.progress - 128) * 2);
			h = PatternHelpers.hue_next[0][0] % 360;
		}
	}
	else if ((PatternHelpers.hue_next[0][0] > PatternHelpers.hue[0][0]) && ((PatternHelpers.hue_next[0][0] - PatternHelpers.hue[0][0]) >= 120)) {
		if (PatternHelpers.progress < 128) {
			s = PatternHelpers.lerp(PatternHelpers.saturation[0][0], 0, PatternHelpers.progress * 2);
			h = PatternHelpers.hue[0][0] % 360;
		}
		else {
			s = PatternHelpers.lerp(0, PatternHelpers.saturation_next[0][0], (PatternHelpers.progress - 128) * 2);
			h = PatternHelpers.hue_next[0][0] % 360;
		}
	}
	else
	{
		s = PatternHelpers.lerp(PatternHelpers.saturation[0][0], PatternHelpers.saturation_next[0][0], PatternHelpers.progress);
		h = PatternHelpers.lerp(PatternHelpers.hue[0][0], PatternHelpers.hue_next[0][0],PatternHelpers.progress) % 360;
	}

	//h = PatternHelpers.lerp(PatternHelpers.hue[0][0], PatternHelpers.hue_next[0][0], PatternHelpers.progress) % 360;

	v = PatternHelpers.lerp(PatternHelpers.value[0][0], PatternHelpers.value_next[0][0], PatternHelpers.progress);
	PatternHelpers.hue_next[0][0] = PatternHelpers.hue_next[0][0] % 360;
	PatternHelpers.hue[0][0] = PatternHelpers.hue[0][0] % 360;
	if (low_glow) {
		Lumen.leds->set_level_hsv(0,0, 0, 0);
		Lumen.leds->set_level_hsv(1,0, 0, 0);
		Lumen.leds->set_level_hsv(2,0, 0, 0);
		Lumen.leds->set_level_hsv(3,0, 0, 0);
		Lumen.leds->set_level_hsv(4,0, 0, 0);
		Lumen.leds->set_level_hsv(5,0, 0, 0);
		Lumen.leds->set_level_hsv(6,0, 0, 0);
		Lumen.leds->set_level_hsv(7,h, s, v * 0.6);
		Lumen.leds->set_level_hsv(8,h, s, v * 0.75);
		Lumen.leds->set_level_hsv(9,h, s, v);
	}
	else
		Lumen.leds->set_solid_hsv(h, s, v);
	if (PatternHelpers.progress == 255) {
		PatternHelpers.value[0][0] = PatternHelpers.value_next[0][0];
		finished = true;
		PatternHelpers.hue[0][0] = PatternHelpers.hue_next[0][0];
		PatternHelpers.saturation[0][0] = PatternHelpers.saturation_next[0][0];
	}
}
void CPatternLight::set_pattern_type(int new_type) {
	type = new_type;
	switch(type) {
		case kPatternLightTypeCandle:
			this->setCandle(); break;
		case kPatternLightTypeIncandescent:
			this->setIncandescent(); break;
		case kPatternLightTypeBlueSky:
			this->setBlueSky(); break;
		case kPatternLightTypeFluorescent:
			this->setFluorescent(); break;
		case kPatternLightTypeSunlight:
			this->setSunlight(); break;
	}
	PatternHelpers.progress = 0;
	this->transition();
	finished = false;

}

void CPatternLight::set_pattern_brightness(int new_brightness) {
	PatternHelpers.value_next[0][0] = new_brightness;
	PatternHelpers.value[0][0] = v;
	finished = false;
	PatternHelpers.progress = 0;

}

void CPatternLight::set_pattern_hue(int new_hue) {
	PatternHelpers.hue[0][1] = new_hue;
	changes_ready = changes_ready | 0x01;
}

void CPatternLight::set_pattern_saturation(int new_saturation) {
	PatternHelpers.saturation[0][1] = new_saturation;
	changes_ready = changes_ready | 0x02;
}

void CPatternLight::set_pattern_glow(bool is_low_glow) {
	finished = true;
	low_glow = is_low_glow;
	if (is_low_glow) {
		Lumen.leds->set_solid_hsv(0, 0, 0);
		Lumen.leds->set_level_hsv(7,PatternHelpers.hue_next[0][0], PatternHelpers.saturation_next[0][0], PatternHelpers.value_next[0][0] * 0.6);
		Lumen.leds->set_level_hsv(8,PatternHelpers.hue_next[0][0], PatternHelpers.saturation_next[0][0], PatternHelpers.value_next[0][0] * 0.75);
		Lumen.leds->set_level_hsv(9,PatternHelpers.hue_next[0][0], PatternHelpers.saturation_next[0][0], PatternHelpers.value_next[0][0]);
	}
	else {
		Lumen.leds->set_solid_hsv(PatternHelpers.hue_next[0][0], PatternHelpers.saturation_next[0][0], PatternHelpers.value_next[0][0]);
	}

}

uint32_t CPatternLight::get_pattern_id() {
	return 0x00000002;
}

void CPatternLight::handle_config_data(uint8_t config_id, uint32_t data) {
	switch(config_id) {
		case kPatternLightConfigSetType: this->set_pattern_type((int)data); break;
		case kPatternLightConfigSetBrightness: this->set_pattern_brightness((int)data); break;
		case kPatternLightConfigSetHue: this->set_pattern_hue((int)data); break;
		case kPatternLightConfigSetSaturation: this->set_pattern_saturation((int)data); break;
		case kPatternLightConfigSetGlow: this->set_pattern_glow((bool)data); break;
	}
}

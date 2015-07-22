/*
 * PatternLight.h
 *
 *  Created on: Jan 26, 2014
 *      Author: billy
 */

#ifndef PATTERNLIGHT_H_
#define PATTERNLIGHT_H_
#include "IPattern.h"
#include "Lumen.h"

typedef enum {
	kPatternLightConfigSetType = 0x01,
	kPatternLightConfigSetBrightness,
	kPatternLightConfigSetHue,
	kPatternLightConfigSetSaturation,
	kPatternLightConfigSetGlow
} kPatternLightConfig;

typedef enum {
	kPatternLightTypeCandle = 0x01,
	kPatternLightTypeIncandescent,
	kPatternLightTypeFluorescent,
	kPatternLightTypeBlueSky,
	kPatternLightTypeSunlight,
	kPatternLightTypeColor
} kPatternLightType;

class CPatternLight : public IPattern {
private:
	uint8_t low_glow= 0;
	uint8_t changes_ready;
	uint16_t h, s, v;
	uint8_t type;

	bool finished= true;
	void setCandle();
	void setIncandescent();
	void setBlueSky();
	void setFluorescent();
	void setSunlight();
	void setHue();
	void transition();
	void set_pattern_type(int new_type);
	void set_pattern_brightness(int new_brightness);
	void set_pattern_hue(int new_hue);
	void set_pattern_saturation(int new_saturation);
	void set_pattern_glow(bool is_low_glow);
public:
    void start(LumenMoodConfig * configs, bool restore_to_defaults);
    void step();
    uint32_t get_pattern_id();
    void handle_config_data(uint8_t config_id, uint32_t data);
    void get_pattern_config(LumenMoodConfig * configs);
};


extern CPatternLight PatternLight;

#endif /* PATTERNLIGHT_H_ */

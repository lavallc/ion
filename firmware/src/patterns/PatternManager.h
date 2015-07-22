/*
 * PatternManager.h
 *
 *  Created on: Jan 24, 2014
 *      Author: billy
 */

#ifndef PATTERNMANAGER_H_
#define PATTERNMANAGER_H_
#include <list>
#include "IPattern.h"
#include "Lumen.h"


#include "PatternOff.h"
#include "PatternLight.h"
#include "PatternDigitalRain.h"
#include "PatternFlame.h"
#include "PatternSpiral.h"
#include "PatternRave.h"
#include "PatternWeather.h"
#include "PatternStrobe.h"
#include "PatternRainbow.h"
#include "PatternLines.h"
#include "PatternHourglass.h"
#include "PatternSparkle.h"
#include "PatternLava.h"
#include "PatternFireworks.h"
#include "PatternBinaryClock.h"
#include "PatternPlasma.h"
#include "PatternIon.h"
#include "PatternPulse.h"
#include "PatternRaw.h"
#include "PatternWhirlpool.h"
#include "PatternVolume.h"
#include "PatternTemperature.h"
#include "NotificationManager.h"
#include "BluetoothManager.h"
#include "PacketFactory.h"
#include "PacketTypes.h"
#include "FileHandler.h"

using namespace std;


class CPatternManager {
private:
	list<IPattern*>::iterator current_pattern;
	list<IPattern*> patterns;
	LumenMoodConfig current_mood_config;
	uint8_t pattern_index = 0;
	uint8_t leash_pattern;
	bool raw_mode = false;
	uint8_t shuffle_min, shuffle_hour, shuffle_second;

public:
	bool is_pattern_off();
	void init();
	void set_raw_mode(bool enabled);
	void set_leash_state(bool leash_state);
	bool save_mood_config();
 	int set_current_pattern(uint32_t pattern_id);
 	uint32_t get_current_pattern();
 	IPattern* get_pattern_with_id(uint32_t pattern_id);
 	uint8_t get_rotation(uint8_t rotation_array[], uint8_t max_patterns);
 	uint32_t getMoodConfig(uint8_t config_id);
 	void step();
	void next();
	void start_pattern(bool load_config);
	void restart_pattern();
};


extern CPatternManager PatternManager;



#endif /* PATTERNMANAGER_H_ */

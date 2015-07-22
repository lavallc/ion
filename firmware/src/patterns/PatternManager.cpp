/*
 * PatternManager.cpp
 *
 *  Created on: Jan 24, 2014
 *      Author: billy
 */

#include "PatternManager.h"
#include "NotificationRetriever.h"
#include "LumenSettings.h"
#include "BondManager.h"
CPatternManager PatternManager;

void CPatternManager::init() {
	//setup patterns array

	//Light
	patterns.push_back(&PatternLight);

	//Music
	patterns.push_back(&PatternVolume);

	patterns.push_back(&PatternRave);
	patterns.push_back(&PatternWhirlpool);

	//Ambiance
	patterns.push_back(&PatternDigitalRain);
	patterns.push_back(&PatternFlame);
	patterns.push_back(&PatternSpiral);
	patterns.push_back(&PatternStrobe);
	patterns.push_back(&PatternRainbow);
	patterns.push_back(&PatternLines);
	patterns.push_back(&PatternSparkle);
	patterns.push_back(&PatternLava);
	patterns.push_back(&PatternFireworks);
	patterns.push_back(&PatternPlasma);
	patterns.push_back(&PatternPulse);

	//Functional
	patterns.push_back(&PatternWeather);
	patterns.push_back(&PatternTemperature);
	patterns.push_back(&PatternHourglass);

	//Todo finish these patterns
	//patterns.push_back(&PatternIon);

	patterns.push_back(&PatternRaw);

	//Off
	patterns.push_back(&PatternOff);

	//grab first pattern
	current_pattern = patterns.begin();
	start_pattern(true);

	//add capsense touch handler
	Lumen.capsense->add_on_touch_handler([&](){
		// only jump to the next pattern if we're not displaying a notification
		if (!NotificationManager.notification_is_active()) {
			PatternManager.next();
			// tell a connected device that the mood has changed
			if (BluetoothManager.device_is_connected())
				PacketFactory.writeMood(kLumenRequestIdFromLamp, kLumenPacketOpCodeGetCurrentMood, PatternManager.get_current_pattern() & 0xFF);
		}
	});
}

bool CPatternManager::is_pattern_off() {
	return (*current_pattern)->get_pattern_id() == 1;
}

void CPatternManager::set_raw_mode(bool enabled) {
	if (raw_mode && !enabled)
		start_pattern(true);
	else if (!raw_mode && enabled) {
		IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
		pattern->start(NULL, false);
	}

	raw_mode = enabled;
}

int CPatternManager::set_current_pattern(uint32_t pattern_id) {
	if (pattern_id == 0x13) {
		set_raw_mode(true);
		return 0;
	}

	set_raw_mode(false);
	int retval = -1;
	list<IPattern*>::iterator p;
	//lets iterator through all the patterns
	for(p = patterns.begin(); p != patterns.end(); p++) {
		//if we find our pattern_id
		if( (*p)->get_pattern_id() == pattern_id) {
			//set it to current_pattern and return
			current_pattern = p;
			retval = 0;
			break;
		}
	}

	for(int i = 0; i < 18; i++) {
		if (LumenSettings.get_mood(i) == (uint8_t)pattern_id) {
			pattern_index = i;
			break;
		}
		else
			pattern_index = 18; //off
	}
	//if we make it here, the pattern wasn't found
	start_pattern(true);
	return retval;
}

// handed an array and the max length for that array for rotation IDs to be returned to.
// Returns the number of patterns placed into the array
uint8_t CPatternManager::get_rotation(uint8_t rotation_array[], uint8_t max_patterns) {
	uint8_t length = 0;
	for (int i = 0; i < max_patterns; i++) {
		rotation_array[i] = LumenSettings.get_mood(i);
		if (rotation_array[i] != 0)
			length++;
	}
	return length;
}

uint32_t CPatternManager::get_current_pattern() {
	return (*current_pattern)->get_pattern_id();
}

uint32_t CPatternManager::getMoodConfig(uint8_t config_id) {
	current_mood_config.mood_id = (uint8_t)(*current_pattern)->get_pattern_id();
	(*current_pattern)->get_pattern_config(&current_mood_config);
	for (int i = 0; i < 10; i++) {
		if (current_mood_config.config_data[i].config_id == config_id)
			return current_mood_config.config_data[i].data;
	}
	return 0;
}

IPattern* CPatternManager::get_pattern_with_id(uint32_t pattern_id) {
	for(list<IPattern*>::iterator p = patterns.begin(); p != patterns.end(); p++) {
		//if we find our pattern_id
		if( (*p)->get_pattern_id() == pattern_id) {
			return (*p);
		}
	}
	return NULL;
}



void CPatternManager::step() {
	if (raw_mode) { //Raw mode takes precedence, but shouldn't alter any other data.
		IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
		pattern->step();
	}
	else {	//Standard step
		(*current_pattern)->step();
		if (!is_pattern_off() && LumenSettings.is_shuffle_enabled()) { //Shuffle Handling
			if ((LumenSettings.is_shuffle_updated()) ||
				(LumenClock.get_minutes() >= shuffle_min &&  LumenClock.get_hours_24() >=  shuffle_hour && LumenClock.get_seconds() >= shuffle_second)) {
				uint8_t shuffle_pattern;
				do  {
					shuffle_pattern = LumenSettings.get_mood(rand() % LumenSettings.get_number_of_moods());
				}
				while(get_current_pattern() == shuffle_pattern);

				set_current_pattern(shuffle_pattern);
				if (BluetoothManager.device_is_connected())
					PacketFactory.writeMood(kLumenRequestIdFromLamp, kLumenPacketOpCodeGetCurrentMood, shuffle_pattern);
			}
		}
	}
}

bool CPatternManager::save_mood_config() {
	current_mood_config.mood_id = (uint8_t)(*current_pattern)->get_pattern_id();
	(*current_pattern)->get_pattern_config(&current_mood_config);
	uint8_t mood_handle[6];
	mood_handle[0] = current_mood_config.mood_id;
	mood_handle[1] = 0;
	mood_handle[2] = 0;
	mood_handle[3] = 0;
	mood_handle[4] = 0;
	mood_handle[5] = 0;
	if (current_mood_config.mood_id == 0x07 || current_mood_config.mood_id == 0x15) { //If we save Whirpool or Music, make sure to save bass_boost with global settings.
		LumenSettings.save_sys_settings();
	}
	if (FileHandler.save_data((uint32_t *)&current_mood_config,mood_handle,sizeof(LumenMoodConfig), kMoodSettingsPage))
		return true;
	else
		return false;
}

void CPatternManager::start_pattern(bool load_config) {

	if (LumenSettings.is_shuffle_enabled())
		shuffle_second = LumenClock.get_seconds();
		shuffle_min = LumenClock.get_minutes() + LumenSettings.get_shuffle_minutes();
	if (shuffle_min >= 60) {
		shuffle_hour = LumenClock.get_hours_24() + shuffle_min / 60;
		shuffle_min % 60;
	}

	if (load_config) {
		uint32_t ptr_next_mood_config;
		LumenMoodConfig  next_mood_config;
		uint8_t mood_handle[6];
		mood_handle[0] = (uint8_t)(*current_pattern)->get_pattern_id();
		mood_handle[1] = 0;
		mood_handle[2] = 0;
		mood_handle[3] = 0;
		mood_handle[4] = 0;
		mood_handle[5] = 0;
		bool success = FileHandler.load_data(&ptr_next_mood_config, mood_handle,kMoodSettingsPage);
		if (success) {
			memcpy((uint8_t *)&next_mood_config, (uint8_t *)ptr_next_mood_config, sizeof(LumenMoodConfig));
			next_mood_config.mood_id = (*current_pattern)->get_pattern_id();
			(*current_pattern)->start(&next_mood_config, false);
			Serial.debug_print("PATTERN_LOAD");
		}
		else {
			(*current_pattern)->start(NULL, false);
			Serial.debug_print("PATTERN_NOT");
		}
	}
	else {
		(*current_pattern)->start(NULL, true);
	}
}

void CPatternManager::restart_pattern() {
	set_raw_mode(false);
	LumenMoodConfig  no_config;
	no_config.mood_id = 0xFF;
	(*current_pattern)->start(&no_config, false);
}

void CPatternManager::set_leash_state(bool leash_state) {
	if (leash_state && is_pattern_off()) {
		set_current_pattern(leash_pattern);
	}
	else if (!leash_state) {
		leash_pattern = get_current_pattern();
		set_current_pattern(1);
	}
}


void CPatternManager::next() {
	Serial.debug_print(sizeof(LumenNotification));
	set_raw_mode(false);
	Serial.debug_print("mood");
	Serial.debug_print(sizeof(LumenMoodConfig));
	uint8_t next_pattern;
	if (pattern_index == 17) { //End of rotation
		pattern_index = 18; //set Off
		next_pattern = 1; //Act like we started over, but we still need to go to pattern off.
	}
	else {
		if (pattern_index == 18) //Reached pattern off
			pattern_index = 0; //Change to first pattern
		else
			pattern_index++;
		next_pattern = LumenSettings.get_mood(pattern_index);
	}
	if (next_pattern == 0) {
		pattern_index = 18;//Off
		next_pattern = 1;
	}
	//Get pattern from PatternManager library.
	if(current_pattern == patterns.end()) current_pattern = patterns.begin();
	list<IPattern*>::iterator p;
	//grab pattern from iterator and call start to reset it
	bool good_pattern = false;
	for(p = patterns.begin(); p != patterns.end(); p++) {
		//if we find our pattern_id
		if((*p)->get_pattern_id() == next_pattern) {
			good_pattern = true;
			current_pattern  = p;
			break;
		}
	}
	if (!good_pattern)
		sd_nvic_SystemReset();
	else
		start_pattern(true);
}

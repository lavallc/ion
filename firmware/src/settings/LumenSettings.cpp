/*
 * LumenSettings.cpp
 *
 *  Created on: Jun 17, 2014
 *      Author: ebarch
 */

#include "LumenSettings.h"
#include "Serial.h"
CLumenSettings LumenSettings;

void CLumenSettings::init() {
	bool success;
	uint32_t sys_pointer;
	uint8_t sys_handle[6];
	sys_handle[0] = 0;
	sys_handle[1] = 0;
	sys_handle[2] = 0;
	sys_handle[3] = 0;
	sys_handle[4] = 0;
	sys_handle[5] = 0;
	success = FileHandler.load_data(&sys_pointer, sys_handle, kSystemSettingsPage);
	if (success) {
		memcpy((uint8_t *)&device_settings, (uint8_t*)sys_pointer,sizeof(LumenDeviceSettings));
	} else {
		memset(device_settings.device_name, 0, 18);
		device_settings.device_name[0] = 'i';
		device_settings.device_name[1] = 'o';
		device_settings.device_name[2] = 'n';
		device_settings.device_name_length = 3;
		device_settings.knock_enabled = 1;
		device_settings.shuffle_time = 0;
		device_settings.leash_enabled = 0;
		device_settings.notifications_enabled = 1;
		device_settings.quiet_time_hour_start = 0;
		device_settings.quiet_time_min_start = 0;
		device_settings.quiet_time_hour_end = 8;
		device_settings.quiet_time_min_end = 0;
		device_settings.quiet_time_enabled = 0;
		device_settings.mood_rotation[0] = kMoodLight;
		device_settings.mood_rotation[1] = kMoodFlame;
		device_settings.mood_rotation[2] = kMoodDigitalRain;
		device_settings.mood_rotation[3] = kMoodRainbow;
		device_settings.mood_rotation[4] = kMoodLava;
		device_settings.mood_rotation[5] = kMoodLines;
		device_settings.mood_rotation[6] = kMoodPlasma;
		device_settings.mood_rotation[7] = kMoodSparkle;
		device_settings.mood_rotation[8] = kMoodSpiral;
		device_settings.mood_rotation[9] = kMoodFireworks;
		device_settings.mood_rotation[10] = kMoodPulse;
		device_settings.mood_rotation[11] = kMoodRave;
		device_settings.mood_rotation[12] = kMoodWhirlpool;
		device_settings.mood_rotation[13] = kMoodVolume;
		device_settings.mood_rotation[14] = kNoMood;
		device_settings.mood_rotation[15] = kNoMood;
		device_settings.mood_rotation[16] = kNoMood;
		device_settings.mood_rotation[17] = kNoMood;
		/*
		//device_settings.mood_rotation[4] = kMoodWeather;
		//device_settings.mood_rotation[5] = kMoodTemperature;
		//device_settings.mood_rotation[6] = kMoodHourglass;
		device_settings.mood_rotation[7] = kMoodLava;
		device_settings.mood_rotation[8] = kMoodLines;
		device_settings.mood_rotation[9] = kMoodPlasma;
		device_settings.mood_rotation[10] = kMoodSparkle;
		device_settings.mood_rotation[11] = kMoodSpiral;
		device_settings.mood_rotation[12] = kMoodFireworks;
		//device_settings.mood_rotation[13] = kMoodStrobe;
		device_settings.mood_rotation[14] = kMoodPulse;
		device_settings.mood_rotation[15] = kMoodRave;
		device_settings.mood_rotation[16] = kMoodWhirlpool;
		device_settings.mood_rotation[17] = kMoodVolume;
		*/
		device_settings.bass_boost = true;
		device_settings.oldest_bond_offset = 0;
		device_settings.secold_bond_offset = 0;
		device_settings.secnew_bond_offset = 0;
		device_settings.newest_bond_offset = 0;
	}
	this->bass_boost = (bool)device_settings.bass_boost;
}

uint16_t CLumenSettings::get_oldest_bond_offset() {
	return device_settings.oldest_bond_offset;
}

void CLumenSettings::clear_oldest_bond_offset() {
	device_settings.oldest_bond_offset = device_settings.secold_bond_offset;
	device_settings.secold_bond_offset = device_settings.secnew_bond_offset;
	device_settings.secnew_bond_offset = device_settings.newest_bond_offset;
	device_settings.newest_bond_offset = 0xFFFF;
}

void CLumenSettings::save_bond_offset(uint16_t bond_offset) {

	if (bond_offset == device_settings.oldest_bond_offset) {
		clear_oldest_bond_offset();
		if (device_settings.oldest_bond_offset == 0 || device_settings.oldest_bond_offset == 0xFFFF)
			device_settings.oldest_bond_offset = bond_offset;
		else if (device_settings.secold_bond_offset == 0 || device_settings.secold_bond_offset == 0xFFFF)
			device_settings.secold_bond_offset = bond_offset;
		else if (device_settings.secnew_bond_offset == 0 || device_settings.secnew_bond_offset == 0xFFFF)
			device_settings.secnew_bond_offset = bond_offset;
		else
			device_settings.newest_bond_offset = bond_offset;

	}
	else if (bond_offset == device_settings.secold_bond_offset) {
		device_settings.secold_bond_offset = device_settings.secnew_bond_offset;
		device_settings.secnew_bond_offset = device_settings.newest_bond_offset;
		device_settings.newest_bond_offset = 0xFFFF;
		if (device_settings.secold_bond_offset == 0 || device_settings.secold_bond_offset == 0xFFFF)
				device_settings.secold_bond_offset = bond_offset;
		else if (device_settings.secnew_bond_offset == 0 || device_settings.secnew_bond_offset == 0xFFFF)
			device_settings.secnew_bond_offset = bond_offset;
		else
			device_settings.newest_bond_offset = bond_offset;
	}
	else if (bond_offset == device_settings.secnew_bond_offset) {
			device_settings.secnew_bond_offset = device_settings.newest_bond_offset;
			device_settings.newest_bond_offset = 0xFFFF;
			if (device_settings.secnew_bond_offset == 0 || device_settings.secnew_bond_offset == 0xFFFF)
				device_settings.secnew_bond_offset = bond_offset;
			else
				device_settings.newest_bond_offset = bond_offset;
	}
	else if (bond_offset == device_settings.newest_bond_offset) {
		device_settings.newest_bond_offset = bond_offset;
	}
	else {
		if (device_settings.oldest_bond_offset == 0 || device_settings.oldest_bond_offset == 0xFFFF)
			device_settings.oldest_bond_offset = bond_offset;
		else if (device_settings.secold_bond_offset == 0 || device_settings.secold_bond_offset == 0xFFFF)
			device_settings.secold_bond_offset = bond_offset;
		else if (device_settings.secnew_bond_offset == 0 || device_settings.secnew_bond_offset == 0xFFFF)
			device_settings.secnew_bond_offset = bond_offset;
		else
			device_settings.newest_bond_offset = bond_offset;
	}
}

bool CLumenSettings::is_knock_enabled() {
	return (bool)device_settings.knock_enabled;
}

bool CLumenSettings::is_shuffle_enabled() {
	return (device_settings.shuffle_time > 0);
}

uint8_t CLumenSettings::get_shuffle_minutes() {
	return device_settings.shuffle_time;
}

bool CLumenSettings::is_shuffle_updated() {
	if (shuffle_updated) {
		shuffle_updated = false;
		return true;
	}
	else
		return false;

}


void CLumenSettings::set_quiet_time(uint8_t hour_start, uint8_t min_start, uint8_t hour_end, uint8_t min_end) {
	device_settings.quiet_time_hour_start = hour_start;
	device_settings.quiet_time_min_start = min_start;
	device_settings.quiet_time_hour_end = hour_end;
	device_settings.quiet_time_min_end = min_end;
}

void CLumenSettings::get_quiet_time(uint8_t * hour_start, uint8_t * min_start, uint8_t * hour_end, uint8_t * min_end) {
	*hour_start = device_settings.quiet_time_hour_start;
	*min_start = device_settings.quiet_time_min_start;
	*hour_end = device_settings.quiet_time_hour_end;
	*min_end = device_settings.quiet_time_min_end;
}
bool CLumenSettings::is_quiet_time_enabled() {
	return device_settings.quiet_time_enabled;
}

bool CLumenSettings::is_notification_enabled() {
	return (bool)device_settings.notifications_enabled;
}

void CLumenSettings::set_quiet_time_enabled(bool is_quiet_time_enabled) {
	device_settings.quiet_time_enabled = (uint8_t)is_quiet_time_enabled;
}
void CLumenSettings::set_notification_enabled(bool is_enabled) {
	device_settings.notifications_enabled = (uint8_t) is_enabled;
}

bool CLumenSettings::is_leash_enabled() {
	return (bool)device_settings.leash_enabled;
}

char * CLumenSettings::get_device_name() {
	return device_settings.device_name;
}

uint8_t CLumenSettings::get_device_name_length() {
	return device_settings.device_name_length;
}

void CLumenSettings::set_device_name(char * deviceName, uint8_t length) {
	memcpy(device_settings.device_name, deviceName, length);
	device_settings.device_name_length = length;
	save_sys_settings();
}

void CLumenSettings::set_settings(bool knock, uint8_t shuffle, bool leash) {
	device_settings.knock_enabled = (uint8_t)knock;
	if (shuffle != device_settings.shuffle_time)
		shuffle_updated = true;
	device_settings.shuffle_time = shuffle;
	device_settings.leash_enabled = (uint8_t)leash;
	save_sys_settings();
}

void CLumenSettings::set_mood_rotation(uint8_t * new_rotation, uint8_t length) {

	for (int i = 0; i < 18; i++) {
		if (i < length)
			device_settings.mood_rotation[i] = new_rotation[i];
		else
			device_settings.mood_rotation[i] = 0;
	}

	save_sys_settings();
}

uint8_t CLumenSettings::get_mood(uint8_t index) {
		return device_settings.mood_rotation[index];
}

uint8_t CLumenSettings::get_number_of_moods() {
	uint8_t num_moods = 0;
	for (int i = 0; i < 18; i++) {
		if (device_settings.mood_rotation[i] != 0)
			num_moods++;
	}
	return num_moods;
}

void CLumenSettings::save_sys_settings() {
	bool success;
	device_settings.bass_boost = (uint8_t)this->bass_boost;
	uint8_t sys_handle[6];
	sys_handle[0] = 0;
	sys_handle[1] = 0;
	sys_handle[2] = 0;
	sys_handle[3] = 0;
	sys_handle[4] = 0;
	sys_handle[5] = 0;
	success = FileHandler.save_data((uint32_t*)&device_settings,sys_handle,sizeof(LumenDeviceSettings),kSystemSettingsPage);
	if (success)
		Serial.debug_print("SYS_SAVED");
	else
		Serial.debug_print("SYS_!SAVED");
}

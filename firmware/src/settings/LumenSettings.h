/*
 * LumenSettings.h
 *
 *  Created on: Jun 17, 2014
 *      Author: ebarch
 */

#ifndef LUMENSETTINGS_H_
#define LUMENSETTINGS_H_
#include <cstdint>
#include <string>
#include "FileHandler.h"

enum {
	kNoMood = 0,
	kOffState,
	kMoodLight,
	kMoodDigitalRain,
	kMoodFlame,
	kNotificationHalo,
	kMoodSpiral,
	kMoodRave,
	kMoodWeather,
	kNotificationSpin,
	kMoodStrobe,
	kMoodLava,
	kMoodRainbow,
	kMoodLines,
	kMoodSparkle,
	kMoodTemperature,
	kMoodHourglass,
	kMoodFireworks,
	kMoodPlasma,
	kStateRaw,
	kMoodIon,
	kMoodWhirlpool,
	kNotificationGlow,
	kMoodPulse,
	kMoodVolume,
	kMoodCount,
};

typedef struct {
	char device_name[18];
	uint32_t device_name_length;
	uint8_t mood_rotation[18];
	uint32_t leash_enabled;
	uint8_t knock_enabled;
	uint8_t shuffle_time;
	uint8_t notifications_enabled;
	uint8_t quiet_time_hour_start;
	uint8_t quiet_time_min_start;
	uint8_t quiet_time_hour_end;
	uint8_t quiet_time_min_end;
	uint8_t quiet_time_enabled;
	uint8_t bass_boost;
	uint16_t oldest_bond_offset;
	uint16_t secold_bond_offset;
	uint16_t secnew_bond_offset;
	uint8_t buffer_data;
	uint32_t newest_bond_offset;

} LumenDeviceSettings;

class CLumenSettings {
public:

	bool bass_boost;
	void init();
	bool is_knock_enabled();
	bool is_shuffle_enabled();
	uint8_t get_number_of_moods();
	uint8_t get_shuffle_minutes();
	bool is_leash_enabled();
	void set_device_name(char * deviceName, uint8_t length);
	char* get_device_name();
	void set_quiet_time(uint8_t hour_start, uint8_t min_start, uint8_t hour_end, uint8_t min_end);
	void get_quiet_time(uint8_t * hour_start, uint8_t * min_start, uint8_t * hour_end, uint8_t * min_end);
	bool is_notification_enabled();
	bool is_quiet_time_enabled();
	bool is_shuffle_updated();
	void set_quiet_time_enabled(bool is_quiet_time_enabled);
	void set_notification_enabled(bool is_enabled);
	void set_mood_rotation(uint8_t * new_rotation, uint8_t length);
	uint8_t get_mood(uint8_t index);
	uint8_t get_device_name_length();
	void set_settings(bool knock, uint8_t shuffle, bool leash);
	void save_sys_settings();
	void save_bond_offset(uint16_t bond_offset);
	void clear_oldest_bond_offset();
	uint16_t get_oldest_bond_offset();
private:
	bool shuffle_updated = false;

	LumenDeviceSettings device_settings;
};

extern CLumenSettings LumenSettings;

#endif /* LUMENSETTINGS_H_ */

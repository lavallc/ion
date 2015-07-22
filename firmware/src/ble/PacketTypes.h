/*
 * PacketTypes.h
 *
 *  Created on: Jan 28, 2014
 *      Author: billy
 */

#ifndef PACKETTYPES_H_
#define PACKETTYPES_H_
#include <cstdint>

#include "LumenLeds.h"

#define kLumenProtocolVersion 0x0001
#define kLumenFirmwareVersion 0x0007

#define kLumenRequestIdFromLamp 0xFF


enum {
	kLumenPacketOpCodeAck = 0x01,
	kLumenPacketOpCodeNak,
	kLumenPacketOpCodeInit,
	kLumenPacketOpCodeSetDeviceName,
	kLumenPacketOpCodeGetDeviceSettings,
	kLumenPacketOpCodeSetDeviceSettings,
	kLumenPacketOpCodeSetWeather,
	kLumenPacketOpCodeSetTime,
	kLumenPacketOpCodeSetMoodConfig,
	kLumenPacketOpCodeGetMoodConfig,
	kLumenPacketOpCodeSaveMoodConfigs,
	kLumenPacketOpCodeRestoreMoodConfigs,
	kLumenPacketOpCodeSetNotificationConfig,
	kLumenPacketOpCodeGetNotificationConfig,
	kLumenPacketOpCodeSetCurrentMood,
	kLumenPacketOpCodeGetCurrentMood,
	kLumenPacketOpCodeTriggerNotification,
	kLumenPacketOpCodeUpdateRotation,
	kLumenPacketOpCodeGetRotation,
	kLumenPacketOpCodeEnterDFUMode,
	kLumenPacketOpCodeShowNotification,
	kLumenPacketOpCodeSetRawSettings,
	kLumenPacketOpCodeBeginBond,
	kLumenPacketOpCodeClearNotification,
	kLumenPacketOpCodeSetRawBank,
	kLumenPacketOpCodeSetRawLED,
	kLumenPacketOpCodeSetRawClearAll,
	kLumenPacketOpCodeSetRawFillColor,
	kLumenPacketOpCodeSetRawRefresh,
	kLumenPacketOpCodeCount,
};

enum {
	kLumenNakInvalidOpCode = 0x01,
	kLumenNakUnknownDeviceType,
	kLumenNakInvalidDeviceName,
	kLumenNakInvalidDeviceSettings,
	kLumenNakInvalidWeather,
	kLumenNakInvalidTime,
	kLumenNakInvalidMoodConfig,
	kLumenNakUnknownMood,
	kLumenNakUnknownMoodConfig,
	kLumenNakInvalidNotificationConfig,
	kLumenNakUnknownNotification,
	kLumenNakInvalidMoodInRotation,
	kLumenNakBondFailed,
	kLumenNakNoBond,
	kLumenNakNoInit,
	kLumenNakCount,
};


typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t payload[18];
} LumenPacket;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t device_type; 		// 0x01 - iOS, 0x02 - Android, 0x03 - Ionode
	uint8_t mac[6];				// passed by android 5+
} LumenPacketInit;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint16_t protocol_version; 	// firmware revision the lamp is on
	uint16_t firmware_version;
	bool bonded;				// are we already bonded?
} LumenPacketFirmwareVersion;


typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	char name[18];
} LumenPacketDeviceName;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketGetDeviceSettings;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	bool knock_enabled;
	bool quiet_time_enabled;
	uint8_t quiet_hour_start;
	uint8_t quiet_min_start;
	uint8_t quiet_hour_end;
	uint8_t quiet_min_end;
	uint8_t shuffle_time;
	bool notifications_enabled;
	bool leash_enabled;
} LumenPacketDeviceSettings;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	int8_t current_temp;
	uint8_t current_conditions;
	int8_t forecast_temp;
	uint8_t forecast_conditions;
	uint8_t sunrise_24hr;
	uint8_t sunrise_min;
	uint8_t sunset_24hr;
	uint8_t sunset_min;
} LumenPacketWeather;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
} LumenPacketTime;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t mood_id;
	uint8_t config_id;
	uint32_t data;
} LumenPacketMoodConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t mood_id;
	uint8_t config_id;
} LumenPacketGetMoodConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketSaveRestoreConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t notification_id;
	bool enabled;
	uint8_t pattern_id;
	uint16_t notification_hue;
	uint8_t notification_value;
	uint8_t notification_saturation;
	uint8_t notification_speed;
	uint8_t notification_duration;
	bool notification_sticky;
} LumenPacketNotificationConfig;


typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t notification_id;
} LumenPacketGetNotificationConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t mood_id;
} LumenPacketMood;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketGetCurrentMood;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t notification_id;
} LumenPacketTriggerNotification;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t moods[18];
} LumenPacketRotation;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketGetEnabledNotifications;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketBeginBond;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketClearNotification;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketGetRotation;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketEnterDFUMode;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketAck;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t response_code;
} LumenPacketNak;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t pattern_id;
	uint16_t notification_hue;
	uint8_t notification_value;
	uint8_t notification_saturation;
	uint8_t notification_speed;
	uint8_t notification_duration;
	bool notification_sticky;
} LumenPacketShowNotificationConfig;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	bool raw_mode_enabled;
} LumenPacketSetRawSettings;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t bank_id;
	RGB led0_color;
	RGB led1_color;
	RGB led2_color;
	RGB led3_color;
	RGB led4_color;
} LumenPacketSetRawBank;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	uint8_t led_id;
	RGB led_color;
} LumenPacketSetRawLED;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketSetRawClearAll;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
	RGB fill_color;
} LumenPacketSetRawFillColor;

typedef struct __attribute__ ((packed)) {
	uint8_t req_id;
	uint8_t op_code;
} LumenPacketSetRawRefresh;



#endif /* PACKETTYPES_H_ */

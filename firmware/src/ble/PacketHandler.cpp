/*
 * PacketHandler.cpp
 *
 *  Created on: Jan 28, 2014
 *      Author: billy
 */

#include "LumenService.h"
#include "PacketHandler.h"
#include "PatternManager.h"
#include "LumenSettings.h"
#include "NotificationManager.h"
#include "LumenClock.h"
#include "LumenWeather.h"
#include "PacketFactory.h"
#include "LumenSettings.h"
#include "BluetoothManager.h"
#include "ANCSHandler.h"
#include "NotificationRetriever.h"
#include "BondManager.h"

extern "C" {
#include "lava_bootloader.h"
}

CPacketHandler PacketHandler;


void CPacketHandler::handle(LumenPacket *packet, uint16_t length) {
	/* TODO: only process packets after the device has sent an init packet */

	if (packet->op_code == kLumenPacketOpCodeInit)
		this->handleInit((LumenPacketInit*)packet, length);
	else {
		if (BondManager.is_inited()) {
			switch(packet->op_code) {
				case kLumenPacketOpCodeSetDeviceName:				this->handleDeviceRename((LumenPacketDeviceName*)packet, length); break;
				case kLumenPacketOpCodeGetDeviceSettings:			this->handleGetDeviceSettings((LumenPacketGetDeviceSettings*)packet, length); break;
				case kLumenPacketOpCodeSetDeviceSettings:			this->handleSetDeviceSettings((LumenPacketDeviceSettings*)packet, length); break;
				case kLumenPacketOpCodeSetWeather:					this->handleSetWeather((LumenPacketWeather*)packet, length); break;
				case kLumenPacketOpCodeSetTime:						this->handleSetTime((LumenPacketTime*)packet, length); break;
				case kLumenPacketOpCodeSetMoodConfig:				this->handleSetMoodConfig((LumenPacketMoodConfig*)packet, length); break;
				case kLumenPacketOpCodeGetMoodConfig:				this->handleGetMoodConfig((LumenPacketGetMoodConfig*)packet, length); break;
				case kLumenPacketOpCodeSaveMoodConfigs:				this->handleSaveMoodConfigs((LumenPacketSaveRestoreConfig*)packet, length); break;
				case kLumenPacketOpCodeRestoreMoodConfigs:			this->handleRestoreMoodConfigs((LumenPacketSaveRestoreConfig*)packet, length); break;
				case kLumenPacketOpCodeSetNotificationConfig:		this->handleSetNotificationConfig((LumenPacketNotificationConfig*)packet, length); break;
				case kLumenPacketOpCodeGetNotificationConfig:		this->handleGetNotificationConfig((LumenPacketGetNotificationConfig*)packet, length); break;
				case kLumenPacketOpCodeSetCurrentMood:				this->handleSetMood((LumenPacketMood*)packet, length); break;
				case kLumenPacketOpCodeGetCurrentMood:				this->handleGetMood((LumenPacketGetCurrentMood*)packet, length); break;
				case kLumenPacketOpCodeTriggerNotification:			this->handleTriggerNotification((LumenPacketTriggerNotification*)packet, length); break;
				case kLumenPacketOpCodeUpdateRotation:				this->handleSetRotation((LumenPacketRotation*)packet, length); break;
				case kLumenPacketOpCodeGetRotation:					this->handleGetRotation((LumenPacketGetRotation*)packet, length); break;
				case kLumenPacketOpCodeEnterDFUMode:				this->handleEnterDFUMode((LumenPacketEnterDFUMode*)packet, length); break;
				case kLumenPacketOpCodeShowNotification:			this->handleShowNotification((LumenPacketShowNotificationConfig*)packet,length); break;
				case kLumenPacketOpCodeBeginBond:					this->handleBeginBond((LumenPacketBeginBond*)packet, length); break;
				case kLumenPacketOpCodeClearNotification:			this->handleClearNotification((LumenPacketClearNotification*)packet, length); break;

				// raw mode packets
				//case kLumenPacketOpCodeSetRawSettings:				this->handleSetRawSettings((LumenPacketSetRawSettings*)packet, length); break;
				case kLumenPacketOpCodeSetRawBank:					this->handleSetRawBank((LumenPacketSetRawBank*)packet, length); break;
				case kLumenPacketOpCodeSetRawLED:					this->handleSetRawLED((LumenPacketSetRawLED*)packet, length); break;
				case kLumenPacketOpCodeSetRawClearAll:				this->handleSetRawClearAll((LumenPacketSetRawClearAll*)packet, length); break;
				case kLumenPacketOpCodeSetRawFillColor:				this->handleSetRawFillColor((LumenPacketSetRawFillColor*)packet, length); break;
				case kLumenPacketOpCodeSetRawRefresh:				this->handleSetRawRefresh((LumenPacketSetRawRefresh*)packet, length); break;

				default: {
					// invalid opcode
					PacketFactory.writeNak(packet->req_id, kLumenNakInvalidOpCode);
					break;
				}
			}

		}
		else {
			PacketFactory.writeNak(packet->req_id, kLumenNakNoInit);
		}
	}
}

void returnAck(uint8_t req_id) {
	PacketFactory.writeAck(req_id);
}


void CPacketHandler::handleInit(LumenPacketInit *packet, uint16_t length) {
	if (packet->device_type > 3)
		PacketFactory.writeNak(packet->req_id, kLumenNakUnknownDeviceType);
	else {
		// init w/o mac address (ios, ionode, and Android < 5.0)
		bool bonded;
		if (length == 3)
			bonded = BondManager.set_device_type(packet->device_type, (uint8_t*)0, 0);
		else
			bonded = BondManager.set_device_type(packet->device_type, packet->mac, 6);
		Serial.debug_print("init");
		Serial.debug_print(bonded);
		/*if (bonded)
			PatternManager.set_current_pattern(0x03);
		else
			PatternManager.set_current_pattern(0x04);*/
		PacketFactory.writeFirmwareVersion(packet->req_id,kLumenProtocolVersion, kLumenFirmwareVersion, bonded);
	}
}

void CPacketHandler::handleClearNotification(LumenPacketClearNotification *packet, uint16_t length) {
	NotificationManager.clear_notification();
	PacketFactory.writeAck(packet->req_id);
}

void CPacketHandler::handleDeviceRename(LumenPacketDeviceName *packet, uint16_t length) {
	// make sure the new name is ASCII
	for (int i=0; i<length-2; i++) {
		if (!isprint(packet->name[i])) {
			PacketFactory.writeNak(packet->req_id, kLumenNakInvalidDeviceName);
			return;
		}
	}

	BluetoothManager.rename_device(packet->name, length-2);
	LumenSettings.set_device_name(packet->name, length-2);
	PacketFactory.writeDeviceName(packet->req_id, packet->name, length-2);
}

void CPacketHandler::handleGetDeviceSettings(LumenPacketGetDeviceSettings *packet, uint16_t length) {
	uint8_t hour_start, min_start, hour_end, min_end;
	LumenSettings.get_quiet_time(&hour_start, &min_start, &hour_end, &min_end);
	PacketFactory.writeDeviceSettings(packet->req_id, packet->op_code, LumenSettings.is_knock_enabled(), LumenSettings.is_quiet_time_enabled(),hour_start, min_start, hour_end, min_end,
			LumenSettings.get_shuffle_minutes(), LumenSettings.is_notification_enabled(), LumenSettings.is_leash_enabled());
}

void CPacketHandler::handleSetDeviceSettings(LumenPacketDeviceSettings *packet, uint16_t length) {
	// packet sanity check
	if (length != sizeof(LumenPacketDeviceSettings))
		return PacketFactory.writeNak(packet->req_id, kLumenNakInvalidDeviceSettings);

	// update settings
	LumenSettings.set_notification_enabled(packet->notifications_enabled);
	LumenSettings.set_quiet_time(packet->quiet_hour_start, packet->quiet_min_start, packet->quiet_hour_end, packet->quiet_min_end);
	LumenSettings.set_settings(packet->knock_enabled, packet->shuffle_time, packet->leash_enabled);
	LumenSettings.set_quiet_time_enabled(packet->quiet_time_enabled);
	// respond to the device with the current settings
	PacketFactory.writeDeviceSettings(packet->req_id, packet->op_code, LumenSettings.is_knock_enabled(), LumenSettings.is_quiet_time_enabled(), packet->quiet_hour_start, packet->quiet_min_start,
			packet->quiet_hour_end, packet->quiet_min_end, LumenSettings.get_shuffle_minutes(), LumenSettings.is_notification_enabled(), LumenSettings.is_leash_enabled());
}

void CPacketHandler::handleSetWeather(LumenPacketWeather *packet, uint16_t length) {
	// packet sanity check
	if (length != sizeof(LumenPacketWeather))
		return PacketFactory.writeNak(packet->req_id, kLumenNakInvalidWeather);

	// update weather
	LumenWeather.set_weather(packet->current_temp, packet->current_conditions, packet->forecast_temp, packet->forecast_conditions);
	// update sun/moon cycle
	LumenWeather.set_sun_times(packet->sunrise_24hr, packet->sunrise_min, packet->sunset_24hr, packet->sunset_min);

	// respond w/ ack
	PacketFactory.writeAck(packet->req_id);
}

void CPacketHandler::handleSetTime(LumenPacketTime *packet, uint16_t length) {
	// packet sanity check
	if (length != sizeof(LumenPacketTime))
		return PacketFactory.writeNak(packet->req_id, kLumenNakInvalidTime);

	// update clock
	LumenClock.set_clock(packet->sec, packet->min, packet->hour);

	// respond w/ ack
	PacketFactory.writeAck(packet->req_id);
}

void CPacketHandler::handleSetMoodConfig(LumenPacketMoodConfig *packet, uint16_t length) {
	// packet sanity check
	if (length != sizeof(LumenPacketMoodConfig))
		return PacketFactory.writeNak(packet->req_id, kLumenNakInvalidMoodConfig);

	IPattern *pattern = PatternManager.get_pattern_with_id(packet->mood_id);

	if (pattern != NULL) {
		pattern->handle_config_data(packet->config_id, packet->data);
		PacketFactory.writeMoodConfig(packet->req_id, packet->op_code, packet->mood_id, packet->config_id, packet->data);
	} else {
		// pattern doesn't exist
		PacketFactory.writeNak(packet->req_id, kLumenNakUnknownMood);
	}
}

void CPacketHandler::handleGetMoodConfig(LumenPacketGetMoodConfig *packet, uint16_t length) {

	if (packet->mood_id != PatternManager.get_current_pattern()) { //Can I request information from the pattern or must I load from filehandler.
		if ((packet->mood_id == 0x7 || packet->mood_id == 0x15) && packet->config_id == 4) //special check if we are trying to get bass_boost
				PacketFactory.writeMoodConfig(packet->req_id, packet->op_code, packet->mood_id, packet->config_id,LumenSettings.bass_boost);
		else { //FileHandler route
			uint32_t ptr_mood_config;
			LumenMoodConfig  mood_config;
			uint8_t mood_handle[6];
			mood_handle[0] = (uint8_t)packet->mood_id;
			mood_handle[1] = 0;
			mood_handle[2] = 0;
			mood_handle[3] = 0;
			mood_handle[4] = 0;
			mood_handle[5] = 0;
			bool success = FileHandler.load_data(&ptr_mood_config, mood_handle,kMoodSettingsPage);
			if (success) {
				memcpy((uint8_t *)&mood_config, (uint8_t *)ptr_mood_config, sizeof(LumenMoodConfig));
				for (int i = 0; i < 10; i++) {
					if (mood_config.config_data[i].config_id == packet->config_id) {
						Serial.debug_print("mood");
						Serial.debug_print(packet->mood_id);
						Serial.debug_print("id");
						Serial.debug_print(packet->config_id);
						Serial.debug_print("val");
						Serial.debug_print(mood_config.config_data[i].data);
						PacketFactory.writeMoodConfig(packet->req_id, packet->op_code, packet->mood_id, packet->config_id,mood_config.config_data[i].data);
						break;
					}
				}

			}
			else {
				PacketFactory.writeNak(packet->req_id, kLumenNakUnknownMoodConfig);
			}
		}
	}
	else {
		//Pattern Route
		uint32_t config_val = PatternManager.getMoodConfig(packet->config_id);
		PacketFactory.writeMoodConfig(packet->req_id, packet->op_code, packet->mood_id, packet->config_id, config_val);
	}

}


void CPacketHandler::handleSaveMoodConfigs(LumenPacketSaveRestoreConfig *packet, uint16_t length) {
	if (PatternManager.save_mood_config())
		PacketFactory.writeAck(packet->req_id);
	else
		PacketFactory.writeNak(packet->req_id, kLumenNakInvalidMoodConfig);

}

void CPacketHandler::handleRestoreMoodConfigs(LumenPacketSaveRestoreConfig *packet, uint16_t length) {
	PatternManager.start_pattern(false);
	PacketFactory.writeAck(packet->req_id);
}

void CPacketHandler::handleSetNotificationConfig(LumenPacketNotificationConfig *packet, uint16_t length) {
	uint8_t device_type;
	if (BondManager.ensure_bond(&device_type)) {
		NotificationRetriever.save_notification(packet);
		PacketFactory.writeNotificationConfig(packet->req_id,packet->op_code,packet->notification_id, packet->enabled,
				packet->pattern_id,packet->notification_hue, packet->notification_value, packet->notification_saturation,
				packet->notification_speed,packet->notification_duration, packet->notification_sticky);
	}
	else
		PacketFactory.writeNak(packet->req_id, kLumenNakBondFailed);

}

void CPacketHandler::handleGetNotificationConfig(LumenPacketGetNotificationConfig *packet, uint16_t length) {
	if (BondManager.is_bonded()) {
		LumenNotification reqNotification;
		reqNotification.notification_id = packet->notification_id;
		if (!NotificationRetriever.load_notification(&reqNotification)) {	//Bonded with no notifications
			PacketFactory.writeNak(packet->req_id, kLumenNakInvalidNotificationConfig);
		}
		else {	//Bonded with notifications
			PacketFactory.writeNotificationConfig(packet->req_id,packet->op_code,reqNotification.notification_id, reqNotification.enabled,
					reqNotification.pattern_id,reqNotification.notification_hue, reqNotification.notification_value, reqNotification.notification_saturation,
					reqNotification.notification_speed,reqNotification.notification_duration, reqNotification.notification_sticky);
		}
	}
	else { //Not bonded
		PacketFactory.writeNak(packet->req_id, kLumenNakNoBond);
	}
}

void CPacketHandler::handleSetMood(LumenPacketMood *packet, uint16_t length) {
	// attempt to set mood
	if (PatternManager.set_current_pattern(packet->mood_id) == -1) {
		// pattern doesn't exist
		PacketFactory.writeNak(packet->req_id, kLumenNakUnknownMood);
	} else {
		PacketFactory.writeMood(packet->req_id, packet->op_code, packet->mood_id);
		NotificationManager.clear_notification();
	}
}

void CPacketHandler::handleGetMood(LumenPacketGetCurrentMood *packet, uint16_t length) {
	PacketFactory.writeMood(packet->req_id, packet->op_code, PatternManager.get_current_pattern() & 0xFF);
}

void CPacketHandler::handleTriggerNotification(LumenPacketTriggerNotification *packet, uint16_t length) {
	// hacky test for the time being - treat notification hash as the pattern ID
	if (NotificationManager.set_current_notification(packet->notification_id) == -1) {
		// notification doesn't exist
		PacketFactory.writeNak(packet->req_id, kLumenNakUnknownNotification);
	} else {
		// we're good
		PacketFactory.writeAck(packet->req_id);
	}
}

void CPacketHandler::handleSetRotation(LumenPacketRotation *packet, uint16_t length) {
	bool bad_mood = false;
	for (int i = 0; i < length-2; i++) {
		if (packet->moods[i] >= kMoodCount || packet->moods[i] == kStateRaw)
			bad_mood = true;
	}
	if (bad_mood)
		PacketFactory.writeNak(packet->req_id, kLumenNakInvalidMoodInRotation);
	else {
		uint8_t rotation_array[18];
		LumenSettings.set_mood_rotation(packet->moods, length -2);
		uint8_t num_patterns_in_rotation = PatternManager.get_rotation(rotation_array, 18);
		PacketFactory.writeRotation(packet->req_id, packet->op_code, rotation_array, num_patterns_in_rotation);
	}
}

void CPacketHandler::handleGetRotation(LumenPacketGetRotation *packet, uint16_t length) {
	uint8_t rotation_array[18];
	uint8_t num_patterns_in_rotation = PatternManager.get_rotation(rotation_array, 18);
	PacketFactory.writeRotation(packet->req_id, packet->op_code, rotation_array, num_patterns_in_rotation);
}

void CPacketHandler::handleEnterDFUMode(LumenPacketEnterDFUMode *packet, uint16_t length) {
	sd_triggerBootloader();
}


void CPacketHandler::handleBeginBond(LumenPacketBeginBond *packet, uint16_t length) {
	uint8_t device_type;
	bool bonded = BondManager.ensure_bond(&device_type);
	if(bonded)
			PacketFactory.writeAck(packet->req_id);
	else {
		if (device_type == kDeviceTypeApple)
			ANCSHandler.request_onconnect_callback(&returnAck,packet->req_id); //ANCS will return a ACK when its done bonding.
	    else if (device_type == kDeviceTypeAndroid || device_type == kDeviceTypeIonode)
				PacketFactory.writeNak(packet->req_id, kLumenNakBondFailed);
	}
}

void CPacketHandler::handleShowNotification(LumenPacketShowNotificationConfig *packet, uint16_t length) {
	Serial.debug_print("show");
	LumenNotification preview_notification;
	preview_notification.notification_hue =  packet->notification_hue;
	preview_notification.notification_value = packet->notification_value;
	preview_notification.notification_saturation = packet->notification_saturation;
	preview_notification.notification_speed = packet->notification_speed;
	preview_notification.notification_duration = packet->notification_duration;
	preview_notification.notification_sticky = packet->notification_sticky;
	preview_notification.pattern_id =  packet->pattern_id;
	NotificationManager.preview_notification((uint32_t *)&preview_notification);
	PacketFactory.writeAck(packet->req_id);
}


// raw mode handlers

void CPacketHandler::handleSetRawSettings(LumenPacketSetRawSettings *packet, uint16_t length) {
	PatternManager.set_raw_mode(packet->raw_mode_enabled);
	PacketFactory.writeAck(packet->req_id);
	Serial.debug_print("Set");
}

void CPacketHandler::handleSetRawBank(LumenPacketSetRawBank *packet, uint16_t length) {
	PatternManager.set_raw_mode(true);
	IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
	RGB RGBbank[5];
	memcpy(&RGBbank[0], &packet->led0_color, sizeof(RGB));
	memcpy(&RGBbank[1], &packet->led1_color, sizeof(RGB));
	memcpy(&RGBbank[2], &packet->led2_color, sizeof(RGB));
	memcpy(&RGBbank[3], &packet->led3_color, sizeof(RGB));
	memcpy(&RGBbank[4], &packet->led4_color, sizeof(RGB));
	pattern->handle_config_data(1, packet->bank_id);
	pattern->handle_config_data(2, (uint32_t)RGBbank);
	PacketFactory.writeAck(packet->req_id);
	Serial.debug_print("bank");
}

void CPacketHandler::handleSetRawLED(LumenPacketSetRawLED *packet, uint16_t length) {
	PatternManager.set_raw_mode(true);
	IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
	pattern->handle_config_data(3, packet->led_id);
	pattern->handle_config_data(4, (uint32_t)&packet->led_color);
	PacketFactory.writeAck(packet->req_id);
	Serial.debug_print("LED");
}

void CPacketHandler::handleSetRawClearAll(LumenPacketSetRawClearAll *packet, uint16_t length) {
	PatternManager.set_raw_mode(true);
	IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
	pattern->handle_config_data(6, 0);
	PacketFactory.writeAck(packet->req_id);
	Serial.debug_print("CLR");
}

void CPacketHandler::handleSetRawFillColor(LumenPacketSetRawFillColor *packet, uint16_t length) {
	PatternManager.set_raw_mode(true);
	IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
	pattern->handle_config_data(5, (uint32_t)&packet->fill_color);
	PacketFactory.writeAck(packet->req_id);
	Serial.debug_print("Fill");
}

void CPacketHandler::handleSetRawRefresh(LumenPacketSetRawRefresh *packet, uint16_t length) {
	PatternManager.set_raw_mode(true);
	IPattern *pattern = PatternManager.get_pattern_with_id(0x13);
	pattern->handle_config_data(7, 0);
	PacketFactory.writeAck(packet->req_id);
	Serial.debug_print("Ref");
}

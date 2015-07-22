/*
 * NotificationManager.cpp
 *
 *  Created on: Jun 09, 2014
 *      Author: ebarch
 */

#include "NotificationManager.h"
#include "LumenSettings.h"
#include "PatternManager.h"
#include "PatternSpin.h"
#include "PatternHalo.h"
#include "PatternGlow.h"
#include "LumenClock.h"
#include "Serial.h"
#include <string.h>


CNotificationManager NotificationManager;

void CNotificationManager::init() {
	//Todo separate notifications
	notifications.push_back(&PatternGlow);
	notifications.push_back(&PatternSpin);
	notifications.push_back(&PatternHalo);

	//add capsense touch handler
	Lumen.capsense->add_on_touch_handler([&](){
		if (notification_active) {
			notification_active = false;
			notification_sticky = false;

			// restart the pattern that was running
			PatternManager.restart_pattern();
		}
	});
}

void CNotificationManager::clear_notification() {
	if (notification_is_active()) {
		notification_active = false;
		PatternManager.restart_pattern();
	}
}

bool CNotificationManager::notification_is_active() {
	return notification_active;
}

void CNotificationManager::convert_ancs_notification(uint8_t * notification_array, uint8_t len, uint8_t catagory_id) {
	uint32_t notification_id;
	switch (catagory_id) {
		case BLE_ANCS_CATEGORY_ID_OTHER:
			notification_id = kLumenNotificationCatchAllOthers;
			break;
		case BLE_ANCS_CATEGORY_ID_INCOMING_CALL:
			notification_id = kLumenNotificationIncomingCall;
			break;
		case BLE_ANCS_CATEGORY_ID_MISSED_CALL:
			notification_id = kLumenNotificationMissedCall;
			break;
		case BLE_ANCS_CATEGORY_ID_VOICE_MAIL:
			notification_id = kLumenNotificationVoicemail;
			break;
		case BLE_ANCS_CATEGORY_ID_SCHEDULE:
			notification_id = kLumenNotificationCalendar;
			break;
		case BLE_ANCS_CATEGORY_ID_EMAIL:
			notification_id = kLumenNotificationEmail;
			break;
		default: {
			string notification = string((char *)notification_array, len);
			std::size_t found;
			found = notification.find("SMS");
			if (found!=std::string::npos) { notification_id = kLumenNotificationSMS; break; }

			found = notification.find("google.hangouts");
			if (found!=std::string::npos) { notification_id = kLumenNotificationGoogleHangouts; break; }

			found = notification.find("google.hangouts");
			if (found!=std::string::npos) { notification_id = kLumenNotificationGoogleHangouts; break; }

			found = notification.find("google.Gmail");
			if (found!=std::string::npos) { notification_id = kLumenNotificationEmail; break; }

			found = notification.find("facebook.Facebook");
			if (found!=std::string::npos) { notification_id = kLumenNotificationFacebook; break; }

			found = notification.find("facebook.Messenger");
			if (found!=std::string::npos) { notification_id = kLumenNotificationFacebookMessenger; break; }

			found = notification.find("google.GooglePlus");
			if (found!=std::string::npos) { notification_id = kLumenNotificationGooglePlus; break; }

			found = notification.find("atebits.Tweetie2");
			if (found!=std::string::npos) { notification_id = kLumenNotificationTwitter; break; }

			found = notification.find("whatsapp.WhatsApp");
			if (found!=std::string::npos) { notification_id = kLumenNotificationWhatsApp; break; }

			found = notification.find("toyopagroup.picaboo");
			if (found!=std::string::npos) { notification_id = kLumenNotificationSnapchat; break; }

			found = notification.find("skype.skype");
			if (found!=std::string::npos) { notification_id = kLumenNotificationSkype; break; }

			found = notification.find("kik.chat");
			if (found!=std::string::npos) { notification_id = kLumenNotificationKik; break; }

			found = notification.find("google.GVDialer");
			if (found!=std::string::npos) { notification_id = kLumenNotificationGoogleVoice; break; }

			found = notification.find("burbn.instagram");
			if (found!=std::string::npos) { notification_id = kLumenNotificationInstagram; break; }
		}
			break;
	}
	set_current_notification(notification_id);


}

int CNotificationManager::set_current_notification(uint32_t notification_id) {
	//lets iterator through all the patterns
	bool is_quiet_time = true;
	uint8_t hour_start, hour_end, min_start, min_end;
	LumenSettings.get_quiet_time(&hour_start, &min_start, &hour_end, &min_end);
	uint16_t time_start, time_end, cur_time;
	time_start = (uint16_t)hour_start * 100 + (uint16_t)min_start;
	time_end = (uint16_t)hour_end * 100 + (uint16_t)min_end;
	cur_time = (uint16_t)LumenClock.get_hours_24() * 100 + (uint16_t)LumenClock.get_minutes();
	if (time_start > time_end) {
		if (cur_time < time_start && cur_time > time_end)
			is_quiet_time = false;
	}
	else {
		if (cur_time < time_start || cur_time > time_end)
			is_quiet_time = false;
	}
	notification.notification_id = notification_id;
	if (LumenSettings.is_notification_enabled() && !is_quiet_time) {
		if (NotificationRetriever.load_notification(&notification)) {
			if (notification.enabled) {
				run_notification();
				Serial.debug_print("RAN_NOTIF");
				Serial.debug_print(notification_id);
				return 0;
			}
		}
		Serial.debug_print("NOTIF_FAILED");
		Serial.debug_print(notification_id);
		//if we make it here, the pattern wasn't found
	}

	return -1;
}

void CNotificationManager::preview_notification(uint32_t * notification_preview) {
	//lets iterator through all the patterns
	memcpy(&notification,notification_preview, sizeof(LumenNotification));
	run_notification();
}

void CNotificationManager::run_notification() {
	list<IPattern*>::iterator p;
	//lets iterator through all the patterns
	for(p = notifications.begin(); p != notifications.end(); p++) {
		//if we find our pattern_id
		if( (*p)->get_pattern_id() == notification.pattern_id) {
			//set it to current_pattern and return
			current_notification = p;
			LumenMoodConfig  notif_config;
			notif_config.mood_id = notification.pattern_id;
			notif_config.config_data[0].data = notification.notification_hue;
			notif_config.config_data[1].data = notification.notification_speed;
			notif_config.config_data[2].data = notification.notification_value;
			notif_config.config_data[3].data = notification.notification_saturation;
			(*current_notification)->start(&notif_config, false);
			notification_sticky = notification.notification_sticky;
			time_end = (uint16_t)notification.notification_duration + LumenClock.get_uptime();
			notification_active = true;
		}
	}


}

void CNotificationManager::step() {
	if (LumenClock.get_uptime() < time_end || notification_sticky) {
		(*current_notification)->step();
	}
	else {
		clear_notification();
	}
}

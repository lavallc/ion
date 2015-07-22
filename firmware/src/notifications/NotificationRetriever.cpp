/*
 * NotificationRetriever.cpp
 *
 *  Created on: Jun 17, 2014
 *      Author: ebarch
 */

#include "NotificationRetriever.h"
#include "FileHandler.h"
#include "Serial.h"
CNotificationRetriever NotificationRetriever;

bool CNotificationRetriever::save_notification(LumenPacketNotificationConfig * new_notif) {
	//Might create some sort of packing process?
	uint8_t notif_handle[6];
	notification.notification_id = new_notif->notification_id;
	notification.enabled = (uint8_t)new_notif->enabled;
	notification.pattern_id = new_notif->pattern_id;
	notification.notification_hue =  new_notif->notification_hue;
	notification.notification_speed =  new_notif->notification_speed;
	notification.notification_duration = new_notif->notification_duration;
	notification.notification_value = new_notif->notification_value;
	notification.notification_saturation = new_notif->notification_saturation;
	notification.notification_sticky = (uint8_t)new_notif->notification_sticky;
	bool success;
	notif_handle[0] = (uint8_t)(notification.notification_id & 0x000000FF);
	notif_handle[1] = 0;
	notif_handle[2] = 0;
	notif_handle[3] = 0;
	notif_handle[4] = 0;
	notif_handle[5] = 0;
	success = FileHandler.save_data((uint32_t *)&notification, notif_handle, sizeof(LumenNotification), kNotificationGenericPage);
	if (success) {
		Serial.debug_print("SAVED");
	}
	else {
		Serial.debug_print("!SAVED");
	}
}


bool CNotificationRetriever::load_notification(LumenNotification * saved_notif) {
	bool success;
	uint32_t notif_pointer;
	uint8_t notif_handle[6];
	notif_handle[0] = (uint8_t)(saved_notif->notification_id & 0x000000FF);
	notif_handle[1] = 0;
	notif_handle[2] = 0;
	notif_handle[3] = 0;
	notif_handle[4] = 0;
	notif_handle[5] = 0;
	success = FileHandler.load_data(&notif_pointer, notif_handle, kNotificationGenericPage);

	if (success) {
		//Might create some sort of unpacking process?
		memcpy((uint8_t *)saved_notif, (uint8_t*)notif_pointer,sizeof(LumenNotification));
		Serial.debug_print("LS");
		Serial.debug_print(notif_pointer);
		return true;
	}
	else {
		//If we don't have bonds, then the next layer should handle creating bonds.
		Serial.debug_print("LF");
		return false;
	}
}

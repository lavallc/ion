/*
 * NotificationRetriever.h
 *
 *  Created on: Jun 17, 2014
 *      Author: ebarch
 */

#ifndef NOTIFICATIONRETRIEVER_H_
#define NOTIFICATIONRETRIEVER_H_
#include <cstdint>
#include "PacketTypes.h"
/*typedef struct {
	uint32_t notification_color;
	uint8_t notification_id;
	uint8_t pattern_id;
	uint8_t notification_speed;
	bool notification_sticky;
	bool enabled;
} LumenNotificationConfig;*/


typedef struct {
	uint8_t notification_id;
	uint16_t notification_hue;
	uint8_t notification_value;
	uint8_t notification_saturation;
	uint8_t notification_speed;
	uint8_t notification_duration;
	uint8_t notification_sticky;
	uint8_t enabled;
	uint8_t pattern_id;
} LumenNotification;

class CNotificationRetriever {

	public:
		bool save_notification(LumenPacketNotificationConfig * new_notif);
		bool load_notification(LumenNotification * saved_notif);

	private:
		LumenNotification notification;

};

extern CNotificationRetriever NotificationRetriever;

#endif /* NOTIFICATIONRETRIEVER_H_ */

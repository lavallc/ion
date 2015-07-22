/*
 * NotificationManager.h
 *
 *  Created on: Jun 09, 2014
 *      Author: ebarch
 */

#ifndef NOTIFICATIONMANAGER_H_
#define NOTIFICATIONMANAGER_H_
#include <list>
#include "IPattern.h"
#include "Lumen.h"
#include "NotificationRetriever.h"
#define NOTIF_STEPS 400
using namespace std;

/*Category IDs for iOS notifications. */
typedef enum
{
    BLE_ANCS_CATEGORY_ID_OTHER = 0,
    BLE_ANCS_CATEGORY_ID_INCOMING_CALL,
    BLE_ANCS_CATEGORY_ID_MISSED_CALL,
    BLE_ANCS_CATEGORY_ID_VOICE_MAIL,
    BLE_ANCS_CATEGORY_ID_SOCIAL,
    BLE_ANCS_CATEGORY_ID_SCHEDULE,
    BLE_ANCS_CATEGORY_ID_EMAIL,
    BLE_ANCS_CATEGORY_ID_NEWS,
    BLE_ANCS_CATEGORY_ID_HEALTH_AND_FITNESS,
    BLE_ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE,
    BLE_ANCS_CATEGORY_ID_LOCATION,
    BLE_ANCS_CATEGORY_ID_ENTERTAINMENT
} ble_ancs_category_id_values_t;


enum {
	kLumenNotificationGeneric = 0x01,
	kLumenNotificationCatchAllOthers,
	kLumenNotificationSMS,
	kLumenNotificationMMS,
	kLumenNotificationCalendar,
	kLumenNotificationVoicemail,
	kLumenNotificationGoogleHangouts,
	kLumenNotificationAlarmClock,
	kLumenNotificationIncomingCall,
	kLumenNotificationMissedCall,
	kLumenNotificationEmail,				/* android mail, apple mail, gmail, yahoo mail */
	kLumenNotificationFacebook,
	kLumenNotificationFacebookMessenger,
	kLumenNotificationGooglePlus,
	kLumenNotificationTwitter,
	kLumenNotificationWhatsApp,
	kLumenNotificationSnapchat,
	kLumenNotificationSkype,
	kLumenNotificationKik,
	kLumenNotificationGoogleVoice,
	kLumenNotificationInstagram,
	kLumenNotificationCount,
};

class CNotificationManager {
private:
	list<IPattern*>::iterator current_notification;
	list<IPattern*> notifications;
	bool notification_active = false;
	bool notification_sticky = false;
	unsigned int time_end = 0;
	LumenNotification notification;
	void run_notification();

public:
	void init();
	void clear_notification();
	void convert_ancs_notification(uint8_t * notification_array, uint8_t len, uint8_t catagory_id);
	int set_current_notification(uint32_t notification_id);
	bool notification_is_active();
	void preview_notification(uint32_t * notification_preview);
	void step();
};


extern CNotificationManager NotificationManager;



#endif /* NOTIFICATIONMANAGER_H_ */

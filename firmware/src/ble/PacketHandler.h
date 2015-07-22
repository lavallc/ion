/*
 * PacketHandler.h
 *
 *  Created on: Jan 28, 2014
 *      Author: billy
 */

#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

#include "PacketTypes.h"



class CPacketHandler {

public:
	//packet router
	void handle(LumenPacket *packet, uint16_t length);

	//individual handlers
	void handleInit(LumenPacketInit *packet, uint16_t length);
	void handleDeviceRename(LumenPacketDeviceName *packet, uint16_t length);
	void handleClearNotification(LumenPacketClearNotification *packet, uint16_t length);
	void handleGetDeviceSettings(LumenPacketGetDeviceSettings *packet, uint16_t length);
	void handleSetDeviceSettings(LumenPacketDeviceSettings *packet, uint16_t length);
	void handleSetWeather(LumenPacketWeather *packet, uint16_t length);
	void handleSetTime(LumenPacketTime *packet, uint16_t length);
	void handleSetMoodConfig(LumenPacketMoodConfig *packet, uint16_t length);
	void handleGetMoodConfig(LumenPacketGetMoodConfig *packet, uint16_t length);
	void handleSaveMoodConfigs(LumenPacketSaveRestoreConfig *packet, uint16_t length);
	void handleRestoreMoodConfigs(LumenPacketSaveRestoreConfig *packet, uint16_t length);
	void handleSetNotificationConfig(LumenPacketNotificationConfig *packet, uint16_t length);
	void handleGetNotificationConfig(LumenPacketGetNotificationConfig *packet, uint16_t length);
	void handleSetMood(LumenPacketMood *packet, uint16_t length);
	void handleGetMood(LumenPacketGetCurrentMood *packet, uint16_t length);
	void handleTriggerNotification(LumenPacketTriggerNotification *packet, uint16_t length);
	void handleSetRotation(LumenPacketRotation *packet, uint16_t length);
	void handleGetRotation(LumenPacketGetRotation *packet, uint16_t length);
	void handleEnterDFUMode(LumenPacketEnterDFUMode *packet, uint16_t length);
	void handleBeginBond(LumenPacketBeginBond *packet, uint16_t length);
	void handleShowNotification(LumenPacketShowNotificationConfig *packet, uint16_t length);
	void handleSetRawSettings(LumenPacketSetRawSettings *packet, uint16_t length);
	void handleSetRawBank(LumenPacketSetRawBank *packet, uint16_t length);
	void handleSetRawLED(LumenPacketSetRawLED *packet, uint16_t length);
	void handleSetRawClearAll(LumenPacketSetRawClearAll *packet, uint16_t length);
	void handleSetRawFillColor(LumenPacketSetRawFillColor *packet, uint16_t length);
	void handleSetRawRefresh(LumenPacketSetRawRefresh *packet, uint16_t length);
};

extern CPacketHandler PacketHandler;
#endif /* PACKETHANDLER_H_ */

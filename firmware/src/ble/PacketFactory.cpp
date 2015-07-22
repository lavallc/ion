/*
 * PacketFactory.cpp
 *
 *  Created on: Jun 10, 2014
 *      Author: ebarch
 */

#include "PacketFactory.h"
#include "PacketTypes.h"
#include "LumenService.h"

CPacketFactory PacketFactory;

// since our BTLE stack doesn't retain or copy data, we need to hold it here before sending the pointer
// right now we are assuming that an app will not request more data before it has received a response
static uint8_t pending_outgoing_packet[20];



void CPacketFactory::writeAck(uint8_t req_id) {
	LumenPacketAck packet = {
		req_id,
		kLumenPacketOpCodeAck
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeNak(uint8_t req_id, uint8_t response_code) {
	LumenPacketNak packet = {
		req_id,
		kLumenPacketOpCodeNak,
		response_code
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeFirmwareVersion(uint8_t req_id, uint16_t protocol_version, uint16_t firmware_version, bool bonded) {
	LumenPacketFirmwareVersion packet = {
		req_id,
		kLumenPacketOpCodeInit,
		protocol_version,
		firmware_version,
		bonded
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeDeviceName(uint8_t req_id, char* device_name, uint8_t device_name_length) {
	LumenPacketDeviceName packet;

	// set req id and op code of device name packet
	packet.req_id = req_id;
	packet.op_code = kLumenPacketOpCodeSetDeviceName;

	// copy device name into packet
	memcpy(packet.name, device_name, device_name_length);

	memcpy(pending_outgoing_packet, &packet, 2+device_name_length);
	LumenService.writePacket(pending_outgoing_packet, 2+device_name_length);
}

void CPacketFactory::writeDeviceSettings(uint8_t req_id, uint8_t op_code, bool knock_enabled, bool quiet_time_enabled, uint8_t quiet_hour_start, uint8_t quiet_min_start,
										uint8_t quiet_hour_end, uint8_t quiet_min_end, uint8_t shuffle_time, bool notifications_enabled, bool leash_enabled) {
	LumenPacketDeviceSettings packet = {
		req_id,
		op_code,
		knock_enabled,
		quiet_time_enabled,
		quiet_hour_start,
		quiet_min_start,
		quiet_hour_end,
		quiet_min_end,
		shuffle_time,
		notifications_enabled,
		leash_enabled
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeMoodConfig(uint8_t req_id, uint8_t op_code, uint8_t mood_id, uint8_t config_id, uint32_t data) {
	LumenPacketMoodConfig packet = {
		req_id,
		op_code,
		mood_id,
		config_id,
		data
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeNotificationConfig(uint8_t req_id, uint8_t op_code, uint8_t notification_id, bool enabled,
											 uint8_t pattern_id, uint16_t notification_hue, uint8_t notification_value,
											 uint8_t notification_saturation, uint8_t notification_speed, uint8_t notification_duration,
											 bool notification_sticky) {
	LumenPacketNotificationConfig packet = {
		req_id,
		op_code,
		notification_id,
		enabled,
		pattern_id,
		notification_hue,
		notification_value,
		notification_saturation,
		notification_speed,
		notification_duration,
		notification_sticky
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeMood(uint8_t req_id, uint8_t op_code, uint8_t mood_id) {
	LumenPacketMood packet = {
		req_id,
		op_code,
		mood_id
	};
	memcpy(pending_outgoing_packet, &packet, sizeof(packet));
	LumenService.writePacket(pending_outgoing_packet, sizeof(packet));
}

void CPacketFactory::writeRotation(uint8_t req_id, uint8_t op_code, uint8_t* moods, uint8_t num_moods) {
	LumenPacketRotation packet;

	// set req id and op code
	packet.req_id = req_id;
	packet.op_code = op_code;

	// copy moods into packet
	memcpy(packet.moods, moods, num_moods);

	memcpy(pending_outgoing_packet, &packet, 2+num_moods);
	LumenService.writePacket(pending_outgoing_packet, 2+num_moods);
}

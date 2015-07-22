/*
 * PacketFactory.h
 *
 *  Created on: Jun 10, 2014
 *      Author: ebarch
 */

#ifndef PACKETFACTORY_H_
#define PACKETFACTORY_H_
#include <cstdint>
#include "PacketTypes.h"

class CPacketFactory {
public:
	void writeAck(uint8_t req_id);
	void writeNak(uint8_t req_id, uint8_t response_code);
	void writeFirmwareVersion(uint8_t req_id, uint16_t protocol_version, uint16_t firmware_version, bool bonded);
	void writeDeviceName(uint8_t req_id, char* device_name, uint8_t device_name_length);
	void writeDeviceSettings(uint8_t req_id, uint8_t op_code, bool knock_enabled, bool quiet_time_enabled, uint8_t quiet_hour_start, uint8_t quiet_min_start, uint8_t quiet_hour_end,
							uint8_t quiet_min_end, uint8_t shuffle_time, bool notifications_enabled, bool leash_enabled);
	void writeMoodConfig(uint8_t req_id, uint8_t op_code, uint8_t mood_id, uint8_t config_id, uint32_t data);
	void writeNotificationConfig(uint8_t req_id, uint8_t op_code, uint8_t notification_id, bool enabled,
			 	 	 	 	 	 uint8_t pattern_id, uint16_t notification_hue, uint8_t notification_value,
			 	 	 	 	 	 uint8_t notification_saturation, uint8_t notification_speed, uint8_t notification_duration,
			 	 	 	 	 	 bool notification_sticky);
	void writeMood(uint8_t req_id, uint8_t op_code, uint8_t mood_id);
	void writeRotation(uint8_t req_id, uint8_t op_code, uint8_t* moods, uint8_t num_moods);
};

extern CPacketFactory PacketFactory;

#endif /* PACKETFACTORY_H_ */

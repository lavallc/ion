/*
 * LumenService.cpp
 *
 *  Created on: Jan 28, 2014
 *      Author: billy
 */

#include "LumenService.h"
#include "PacketHandler.h"
#include "Serial.h"
extern "C" {
#include "lava_bootloader.h"

}

CLumenService LumenService;

CLumenService::CLumenService() {
	setUUID(kLumenServiceUUID);
	setName("LumenService");

	_inCharacteristic.setUUID(kLumenServiceInCharacteristic)
					.setName(string("data (phone->lumen)"))
					.setWritable(true);

	_inCharacteristic.onWrite([&](const CharacteristicValue &val) {
		//Serial.print("Characteristic Update (packet length=");
		//Serial.print((int)val.length);
		//Serial.println(")");
		PacketHandler.handle((LumenPacket*)(val.data), val.length);
	});


	_outCharacteristic.setUUID(kLumenServiceOutCharacteristic)
					.setName(string("data (lumen->phone)"))
					.setWritable(false)
					.setNotifies(true);


	addCharacteristic(&_inCharacteristic);
	addCharacteristic(&_outCharacteristic);
}


void CLumenService::writePacket(uint8_t* packet, uint16_t length) {
	CharacteristicValue v;
	v.data = packet;
	v.length = length;
	_outCharacteristic.setCharacteristicValue(v);
}


/*
 * LumenService.h
 *
 *  Created on: Jan 28, 2014
 *      Author: billy
 */

#ifndef LUMENSERVICE_H_
#define LUMENSERVICE_H_

#include <string>
#include "BluetoothLE.h"
#include "PacketTypes.h"
#include "PatternManager.h"

#define kLumenServiceUUID UUID("2e8b0001-c4b7-43f1-bbdd-6285294a4116")
#define kLumenServiceInCharacteristic UUID("2e8b0002-c4b7-43f1-bbdd-6285294a4116")
#define kLumenServiceOutCharacteristic UUID("2e8b0003-c4b7-43f1-bbdd-6285294a4116")

using namespace BLEpp;
using namespace std;

class CLumenService : public GenericService {
  protected:
    CharacteristicT<CharacteristicValue> _inCharacteristic;
    CharacteristicT<CharacteristicValue> _outCharacteristic;

  public:
    CLumenService();
    void writePacket(uint8_t* packet, uint16_t length);
    void log(string s);
};


extern CLumenService LumenService;

#endif /* LUMENSERVICE_H_ */

/*
 * Lumen.h
 *
 *  Created on: Jan 22, 2014
 *      Author: billy
 */

#ifndef LUMEN_H_
#define LUMEN_H_

#include <stdlib.h>

#include "LumenLeds.h"
#include "LumenCapsense.h"
#include "LumenAudio.h"
#include "LumenInterrupts.h"
#include "Serial.h"
#include "LumenClock.h"

class CLumen {
private:

public:
	CLumenLeds 		 *leds;
	CLumenCapsense 	 *capsense;
	CLumenAudio 	 *audio;
	CLumenInterrupts *interrupts;
	CLumenClock 	 *clock;

	volatile bool radio_is_active;
	void init();
	void seed_random();

};

extern CLumen Lumen;

#endif /* LUMEN_H_ */

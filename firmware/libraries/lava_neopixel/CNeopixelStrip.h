//
//  CNeopixelStrip.h
//  firmware
//
//  Created by Billy Lindeman on 1/17/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#ifndef __CNeopixelStrip__
#define __CNeopixelStrip__

#include "NeopixelTimingMacros.h"
#include <stdint.h>
#include <stdbool.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

typedef union {
	struct {
		uint8_t g, r, b;
	} simple;
	uint8_t grb[3];
} color_t;


class CNeopixelStrip {
    
private:
    uint8_t pin_num;
    
public:
    color_t *leds;
    uint16_t num_leds;


    CNeopixelStrip(uint8_t pin_num, uint16_t num_leds);
    ~CNeopixelStrip();
    
    void clear();
    void show();
    uint8_t set_color(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
    uint8_t set_color_and_show(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
    
    
};


#endif /* defined(__CNeopixelStrip__) */

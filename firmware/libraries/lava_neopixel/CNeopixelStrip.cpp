//
//  CNeopixelStrip.cpp
//  firmware
//
//  Created by Billy Lindeman on 1/17/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#include "CNeopixelStrip.h"



CNeopixelStrip::CNeopixelStrip(uint8_t pin_num, uint16_t num_leds) {
	this->leds = new color_t[num_leds];
	this->pin_num = pin_num;
	this->num_leds = num_leds;
	nrf_gpio_cfg_output(pin_num);
	NRF_GPIO->OUTCLR = (1UL << pin_num);
	for (int i = 0; i < num_leds; i++) {
		this->leds[i].simple.g = 0;
		this->leds[i].simple.r = 0;
		this->leds[i].simple.b = 0;
	}
}

void CNeopixelStrip::clear() {
	for (int i = 0; i < this->num_leds; i++) {
		this->leds[i].simple.g = 0;
		this->leds[i].simple.r = 0;
		this->leds[i].simple.b = 0;
	}
	this->show();
}

void CNeopixelStrip::show() {
	const uint8_t PIN = this->pin_num;
	NRF_GPIO->OUTCLR = (1UL << PIN);
	nrf_delay_us(50);
	for (int i = 0; i < this->num_leds; i++) {
		for (int j = 0; j < 3; j++) {
			if ((this->leds[i].grb[j] & 128) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 64) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 32) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 16) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 8) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 4) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 2) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
            
			if ((this->leds[i].grb[j] & 1) > 0) {
				NEOPIXEL_TIMING_MACRO_SEND_ONE
			} else {
				NEOPIXEL_TIMING_MACRO_SEND_ZERO
			}
		}
	}
}

uint8_t CNeopixelStrip::set_color(uint16_t index, uint8_t red,
                           uint8_t green, uint8_t blue) {
	if (index < this->num_leds) {
		this->leds[index].simple.r = red;
		this->leds[index].simple.g = green;
		this->leds[index].simple.b = blue;
	} else
		return 1;
	return 0;
}

uint8_t CNeopixelStrip::set_color_and_show(uint16_t index,
                                    uint8_t red, uint8_t green, uint8_t blue) {
	if (index < this->num_leds) {
		this->leds[index].simple.r = red;
		this->leds[index].simple.g = green;
		this->leds[index].simple.b = blue;
		this->show();
	} else
		return 1;
	return 0;
}

CNeopixelStrip::~CNeopixelStrip() {
	delete leds;
	this->num_leds = 0;
	this->pin_num = 0;
}

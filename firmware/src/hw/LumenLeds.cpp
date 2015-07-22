//
//  LumenLeds.cpp
//  firmware
//
//  Created by Billy Lindeman on 1/18/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#include "LumenLeds.h"
#include <cstddef>
#include "Lumen.h"
#include "math.h"
extern "C" {
	#include "nrf_gpio.h"
}

CLumenLeds LumenLeds;

CLumenLeds::CLumenLeds() {
    ray1 = new CNeopixelStrip(kSunBoardPinRay1, kSunBoardNumberOfLedsPerRay);
    ray2 = new CNeopixelStrip(kSunBoardPinRay2, kSunBoardNumberOfLedsPerRay);
    ray3 = new CNeopixelStrip(kSunBoardPinRay3, kSunBoardNumberOfLedsPerRay);
    ray4 = new CNeopixelStrip(kSunBoardPinRay4, kSunBoardNumberOfLedsPerRay);
}

//returns a strip object for an index (useful for color functions
CNeopixelStrip* CLumenLeds::get_strip_for_ray_index(int index) {
	if (index == 0)
		return ray1;
	else if (index == 1)
		return ray2;
	else if (index == 2)
		return ray3;
	else if (index == 3)
		return ray4;
	return NULL;
}

//sets a single pixel rgb value
void CLumenLeds::set_pixel_rgb(int ray_index, int led_index, uint8_t r,
                              uint8_t g, uint8_t b) {
	if (ray_index > (kSunBoardNumberOfRays - 1))
		return;
	if (led_index > (kSunBoardNumberOfLedsPerRay - 1))
		return;
	CNeopixelStrip *strip = this->get_strip_for_ray_index(ray_index);
	strip->set_color(led_index, r, g, b);
}

//sets a single pixel rgb value
void CLumenLeds::set_pixel_linear_rgb(int ray_index, int led_index, uint8_t r,
                              uint8_t g, uint8_t b) {
	if (ray_index > (kSunBoardNumberOfRays - 1))
		return;
	if (led_index > (kSunBoardNumberOfLedsPerRay - 1))
		return;
	CNeopixelStrip *strip = this->get_strip_for_ray_index(ray_index);
	strip->set_color(led_index, linear[r], linear[g], linear[b]);
}

void CLumenLeds::set_pixel_hsv(int ray_index, int led_index, uint16_t h, uint16_t s,
		uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_pixel_rgb(ray_index, led_index, r, g, b);
}

void CLumenLeds::set_pixel_linear_hsv(int ray_index, int led_index, uint16_t h, uint16_t s,
		uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_pixel_linear_rgb(ray_index, led_index, r, g, b);
}

void CLumenLeds::set_level_rgb(int led_index, uint8_t r, uint8_t g, uint8_t b) {
	if (led_index >= kSunBoardNumberOfLedsPerRay)
		return;
	for (int i = 0; i < kSunBoardNumberOfRays; i++) {
		CNeopixelStrip *strip = this->get_strip_for_ray_index(i);
		strip->set_color(led_index, r, g, b);
	}
}

void CLumenLeds::set_level_linear_rgb(int led_index, uint8_t r, uint8_t g, uint8_t b) {
	if (led_index >= kSunBoardNumberOfLedsPerRay)
		return;
	for (int i = 0; i < kSunBoardNumberOfRays; i++) {
		CNeopixelStrip *strip = this->get_strip_for_ray_index(i);
		strip->set_color(led_index, linear[r], linear[g], linear[b]);
	}
}

void CLumenLeds::set_level_hsv(int led_index, uint16_t h, uint16_t s, uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_level_rgb(led_index, r, g, b);
}

void CLumenLeds::set_level_linear_hsv(int led_index, uint16_t h, uint16_t s, uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_level_linear_rgb(led_index, r, g, b);
}

void CLumenLeds::set_solid_rgb(uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < kSunBoardNumberOfRays; i++) {
		for (int j = 0; j < kSunBoardNumberOfLedsPerRay; j++) {
			this->set_pixel_rgb(i, j, r, g, b);
		}
	}
}

void CLumenLeds::set_solid_linear_rgb(uint8_t r, uint8_t g, uint8_t b) {
	for (int i = 0; i < kSunBoardNumberOfRays; i++) {
		for (int j = 0; j < kSunBoardNumberOfLedsPerRay; j++) {
			this->set_pixel_linear_rgb(i, j, r, g, b);
		}
	}
}

void CLumenLeds::set_solid_hsv(uint16_t h, uint16_t s, uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_solid_rgb(r, g, b);
}

void CLumenLeds::set_solid_linear_hsv(uint16_t h, uint16_t s, uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_solid_linear_rgb(r, g, b);
}

void CLumenLeds::set_ray_rgb(int ray_index, uint8_t r, uint8_t g, uint8_t b) {
	if (ray_index >= kSunBoardNumberOfRays)
		return;
	CNeopixelStrip *strip = this->get_strip_for_ray_index(ray_index);
	for (int i = 0; i < kSunBoardNumberOfLedsPerRay; i++) {
		strip->set_color(i, r, g, b);
	}
}

void CLumenLeds::set_ray_hsv(int ray_index, uint16_t h, uint16_t s, uint16_t v) {
	uint8_t r, g, b;
	this->hsv_to_rgb(h, s, v, &r, &g, &b);
	this->set_ray_rgb(ray_index, r, g, b);
}

void CLumenLeds::show() {
    this->ray1->show();
    this->ray2->show();
    this->ray3->show();
    this->ray4->show();
}

void CLumenLeds::each(led_iterator_callback led_iterator) {
	for(int i=0; i<kSunBoardNumberOfRays; i++) {
		for(int j=0; j<kSunBoardNumberOfLedsPerRay; j++) {
			led_iterator(i,j);
		}
	}
}

void CLumenLeds::hsv_to_rgb(uint16_t h, uint16_t s, uint16_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint32_t region, fpart, p, q, t;

    h = h % 360;

    h = (h * 255) / 360;

    /* make hue 0-5 */
    region = h / 43;
    /* find remainder part, make it from 0-255 */
    fpart = (h - (region * 43)) * 6;

    if (s == 0) {
    	/* color is grayscale */
    	*r = *g = *b = v;
    	return;
    }

    /* calculate temp vars, doing integer multiplication */
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * fpart) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;

    /* assign temp vars based on color cone region */
    switch(region) {
        case 0:
            *r = v; *g = t; *b = p; break;
        case 1:
            *r = q; *g = v; *b = p; break;
        case 2:
            *r = p; *g = v; *b = t; break;
        case 3:
            *r = p; *g = q; *b = v; break;
        case 4:
            *r = t; *g = p; *b = v; break;
        default:
            *r = v; *g = p; *b = q; break;
    }

    return;
}

void CLumenLeds::get_pixel_rgb(int ray_index, int led_index, uint8_t *r,
                              uint8_t *g, uint8_t *b) {
	CNeopixelStrip *strip = this->get_strip_for_ray_index(ray_index);
	*r = strip->leds->simple.r;
	*g = strip->leds->simple.g;
	*b = strip->leds->simple.b;
}



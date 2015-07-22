//
//  LumenLeds.h
//  firmware
//
//  Created by Billy Lindeman on 1/18/14.
//  Copyright (c) 2014 lava. All rights reserved.
//

#ifndef __LumenLeds__
#define __LumenLeds__

#include <functional>
#include "CNeopixelStrip.h"
#include "sun_board.h"

using namespace std;

typedef struct __attribute__ ((packed)) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB;


class CLumenLeds {
private:
	char buffer[40];
    CNeopixelStrip *ray1,*ray2,*ray3,*ray4;
    const unsigned char linear[256] = {
   	0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    	1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
    	2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
    	3, 4, 4, 4, 4, 4, 4, 5, 5, 5,
    	5, 5, 6, 6, 6, 6, 6, 7, 7, 7,
    	7, 8, 8, 8, 8, 9, 9, 9, 10, 10,
    	10, 10, 11, 11, 11, 12, 12, 12, 13, 13,
    	13, 14, 14, 15, 15, 15, 16, 16, 17, 17,
    	17, 18, 18, 19, 19, 20, 20, 21, 21, 22,
    	22, 23, 23, 24, 24, 25, 25, 26, 26, 27,
    	28, 28, 29, 29, 30, 31, 31, 32, 32, 33,
    	34, 34, 35, 36, 37, 37, 38, 39, 39, 40,
    	41, 42, 43, 43, 44, 45, 46, 47, 47, 48,
    	49, 50, 51, 52, 53, 54, 54, 55, 56, 57,
    	58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
    	68, 70, 71, 72, 73, 74, 75, 76, 77, 79,
    	80, 81, 82, 83, 85, 86, 87, 88, 90, 91,
    	92, 94, 95, 96, 98, 99, 100, 102, 103, 105,
    	106, 108, 109, 110, 112, 113, 115, 116, 118, 120,
    	121, 123, 124, 126, 128, 129, 131, 132, 134, 136,
    	138, 139, 141, 143, 145, 146, 148, 150, 152, 154,
    	155, 157, 159, 161, 163, 165, 167, 169, 171, 173,
    	175, 177, 179, 181, 183, 185, 187, 189, 191, 193,
    	196, 198, 200, 202, 204, 207, 209, 211, 214, 216,
    	218, 220, 223, 225, 228, 230, 232, 235, 237, 240,
    	242, 245, 247, 250, 252, 255,
    };

public:
    CLumenLeds();

    typedef function<void(int ray, int index)> led_iterator_callback;

    //returns a strip object for an index (useful for color functions
    CNeopixelStrip* get_strip_for_ray_index(int index);

    //pixel color functions
    void set_pixel_rgb(int ray_index, int led_index, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel_linear_rgb(int ray_index, int led_index, uint8_t r, uint8_t g, uint8_t b);
    void set_pixel_hsv(int ray_index, int led_index, uint16_t h, uint16_t s, uint16_t v);
    void set_pixel_linear_hsv(int ray_index, int led_index, uint16_t h, uint16_t s, uint16_t v);
    void set_level_rgb(int led_index, uint8_t r, uint8_t g, uint8_t b);
    void set_level_linear_rgb(int led_index, uint8_t r, uint8_t g, uint8_t b);
    void set_level_hsv(int led_index, uint16_t h, uint16_t s, uint16_t v);
    void set_level_linear_hsv(int led_index, uint16_t h, uint16_t s, uint16_t v);
    void set_solid_rgb(uint8_t r, uint8_t g, uint8_t b);
    void set_solid_linear_rgb(uint8_t r, uint8_t g, uint8_t b);
    void set_solid_hsv(uint16_t h, uint16_t s, uint16_t v);
    void set_solid_linear_hsv(uint16_t h, uint16_t s, uint16_t v);
    void set_ray_rgb(int ray_index, uint8_t r, uint8_t g, uint8_t b);
    void set_ray_hsv(int ray_index, uint16_t h, uint16_t s, uint16_t v);

    //return the currently stored pixel from the framebuffer
    void get_pixel_rgb(int ray_index, int led_index, uint8_t *r, uint8_t *g, uint8_t *b);


    void each(led_iterator_callback led_iterator);

    //refresh all rays
    void show();
   
    
    //hsv to rgb space
    void hsv_to_rgb(uint16_t h, uint16_t s, uint16_t v, uint8_t *r, uint8_t *g, uint8_t *b);
};

extern CLumenLeds LumenLeds;

#endif /* defined(LumenLeds__) */

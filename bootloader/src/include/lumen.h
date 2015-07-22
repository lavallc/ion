#ifndef LUMEN_H
#define LUMEN_H

#include "sun_board.h"
#include "neopixel.h"


extern uint32_t lumen_dfu_flashed_data;
extern uint32_t lumen_dfu_data_size;
extern double lumen_flash_progress;
extern bool volatile clear_finished;


void lumen_init();
void lumen_attach_notification_handler();
void lumen_radio_active_notification_handler(bool radio_is_active);
neopixel_strip_t *lumen_get_strip_for_ray_index(int ray);
void lumen_set_pixel_rgb(int ray_index, int led_index, uint8_t r, uint8_t g, uint8_t b);
void lumen_set_level_rgb(int led_index, uint8_t r, uint8_t g, uint8_t b);
void lumen_set_solid_rgb(uint8_t r, uint8_t g, uint8_t b);
void lumen_leds_show();
void lumen_capsense_init(void);
void lumen_set_progress(double progress);
bool lumen_isTouched(void);
uint16_t lumen_capsense_read(void);
#endif
//functions for lumen
#include <stdlib.h>
#include "lumen.h"
#include "math.h"
#include "ble_radio_notification.h"
#include "twi_master.h"
#include "mpr121.h"
#include "app_timer.h"
#include "app_util.h"
#include "nrf_gpio.h"

uint32_t lumen_dfu_flashed_data;
uint32_t lumen_dfu_data_size;
double lumen_flash_progress;
bool volatile clear_finished = false;

neopixel_strip_t ray1;
neopixel_strip_t ray2;
neopixel_strip_t ray3;
neopixel_strip_t ray4;

app_sched_event_handler_t lumen_event_show_led;


void lumen_init() {
	//initialize all the ray structures
	neopixel_init(&ray1, kSunBoardPinRay1, kSunBoardNumberOfLedsPerRay);
	neopixel_init(&ray2, kSunBoardPinRay2, kSunBoardNumberOfLedsPerRay);
	neopixel_init(&ray3, kSunBoardPinRay3, kSunBoardNumberOfLedsPerRay);
	neopixel_init(&ray4, kSunBoardPinRay4, kSunBoardNumberOfLedsPerRay);
	lumen_dfu_flashed_data = 0;
  lumen_dfu_data_size = 0;
}

void lumen_task_led_show(void * context, uint16_t size) {


		CRITICAL_REGION_ENTER();
		lumen_leds_show();
		CRITICAL_REGION_EXIT();


}

void lumen_attach_notification_handler() {
		ble_radio_notification_init(NRF_APP_PRIORITY_HIGH,
                              NRF_RADIO_NOTIFICATION_DISTANCE_5500US,
                              &lumen_radio_active_notification_handler);
}

void lumen_destroy() {
	neopixel_destroy(&ray1);
	neopixel_destroy(&ray2);
	neopixel_destroy(&ray3);
	neopixel_destroy(&ray4);
}

void lumen_radio_active_notification_handler(bool radio_is_active) {
	//if the radio is no longer active, send an update to the leds
	if(!radio_is_active){ 	
			nrf_delay_us(250);
			app_sched_event_put(&lumen_event_show_led, sizeof(lumen_event_show_led),lumen_task_led_show);
			
	}
}

//returns a strip object for an index (useful for color functions
neopixel_strip_t *lumen_get_strip_for_ray_index(int ray) {
	if(ray == 0) return &ray1;
	else if(ray == 1) return &ray2;
	else if(ray == 2) return &ray3;
	else if(ray == 3) return &ray4;
	return NULL;
}

//sets a single pixel rgb value
void lumen_set_pixel_rgb(int ray_index, int led_index, uint8_t r, uint8_t g, uint8_t b) {
	if(ray_index > (kSunBoardNumberOfRays-1)) return;
	if(led_index > kSunBoardNumberOfLedsPerRay) return;
	
	neopixel_strip_t *strip = lumen_get_strip_for_ray_index(ray_index);
	neopixel_set_color(strip, led_index, r,g,b);	
}

void lumen_set_level_rgb(int led_index, uint8_t r, uint8_t g, uint8_t b) {
	if(led_index >= kSunBoardNumberOfLedsPerRay) return;
	for(int i=0; i<kSunBoardNumberOfRays; i++) {
		neopixel_strip_t *strip = lumen_get_strip_for_ray_index(i);
		neopixel_set_color(strip, (kSunBoardNumberOfLedsPerRay-1) - led_index, r,g,b);
	}
}

void lumen_set_progress(double progress) {
	int already_on = (int) floor(progress*10);
	double percent_of_next = ((progress*10) - already_on);
	uint8_t next_led_value = percent_of_next*255;
	
	for(int i=0; i<kSunBoardNumberOfLedsPerRay; i++) {
		if(i<already_on) lumen_set_level_rgb(i, 0,0,255);
		else if( i == (already_on) ) lumen_set_level_rgb(i,0,0,next_led_value);
		else lumen_set_level_rgb(i, 0,0,0);
	}
}

void lumen_set_solid_rgb(uint8_t r, uint8_t g, uint8_t b) {
	for(int i=0; i<kSunBoardNumberOfRays; i++) {
		for(int j=0; j< kSunBoardNumberOfLedsPerRay; j++ ) {
				lumen_set_pixel_rgb(i, j, r, g, b);
		}
	}
}
void lumen_leds_show() {
		nrf_delay_us(250);
		neopixel_show(&ray1);
		neopixel_show(&ray2);
		neopixel_show(&ray3);
		neopixel_show(&ray4);

}

void lumen_capsense_init(void)
{
	twi_master_init();
	mpr121_config_prox_touch_0();
}

bool lumen_isTouched(void)
{
	return mpr121_isTouched_electrode(0);
}

uint16_t lumen_capsense_read(void)
{
	uint16_t filtered_data;
	mpr121_read_electrode(0, &filtered_data);
	return filtered_data;
}
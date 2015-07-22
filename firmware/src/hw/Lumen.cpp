/*
 * Lumen.cpp
 *
 *  Created on: Jan 22, 2014
 *      Author: billy
 */



#include "TaskManager.h"
#include "TaskLeds.h"
#include "sun_board.h"
#include "PatternHelpers.h"

extern "C" {
	#include "ble_radio_notification.h"
	void lumen_radio_active_notification_handler(bool radio_is_active);
	#include "nrf_gpio.h"
}


CLumen Lumen;

void CLumen::init() {
	this->leds = &LumenLeds;
	this->audio = &LumenAudio;
	this->capsense = &LumenCapsense;
	this->interrupts = &LumenInterrupts;
	this->clock = &LumenClock;

	//Input pin for capsense interrupt from soldered modules
	NRF_GPIO->PIN_CNF[kSunBoardCapsnseIRQPin] = (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos)
			| (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos)
			| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
			| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);

	//Setup input pin for MSGEQ7 chip output.
	NRF_GPIO->PIN_CNF[kSunBoardEQDataPin] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
			| (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
			| (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)
			| (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);


	//Setup strobe pin to go to MSGEQ7 chip
	nrf_gpio_cfg_output(kSunBoardEQStrobe);
	nrf_gpio_pin_clear(kSunBoardEQStrobe);
	//Setup reset pin to go to MSGEQ7 chip
	nrf_gpio_cfg_output(kSunBoardEQReset);
	nrf_gpio_pin_clear(kSunBoardEQReset);
	//Setup debug pins
	nrf_gpio_cfg_output(kSunBoardDigPin1);
	nrf_gpio_cfg_output(kSunBoardDigPin2);
	nrf_gpio_cfg_output(kSunBoardDigPin3);

	//Disable Laird Whipser Mode
	nrf_gpio_cfg_output(kSunBoardPinLairdWhisperDisable);
	nrf_gpio_pin_clear(kSunBoardPinLairdWhisperDisable);


	this->capsense->init();
	this->interrupts->init();
	this->audio->init();


	//seed PRNG from the hardware-rng
	this->seed_random();

	//init serial
	Serial.begin(115200, kSunBoardFTDISerialTx, kSunBoardFTDISerialRx);

	LumenClock.set_clock(0,30,12);
	PatternHelpers.define_colors[kLumenAudioFrequency160Hz].hue = 0;
	PatternHelpers.define_colors[kLumenAudioFrequency400Hz].hue = 120;
	PatternHelpers.define_colors[kLumenAudioFrequency1000Hz].hue = 240;
	PatternHelpers.define_colors[kLumenAudioFrequency2500Hz].hue = 60;
	PatternHelpers.define_colors[kLumenAudioFrequency6250Hz].hue = 180;
	PatternHelpers.define_colors[kLumenAudioFrequency16000Hz].hue = 300;

	PatternHelpers.define_colors[kLumenAudioFrequency160Hz].value = 50;
	PatternHelpers.define_colors[kLumenAudioFrequency400Hz].value = 50;
	PatternHelpers.define_colors[kLumenAudioFrequency1000Hz].value = 50;
	PatternHelpers.define_colors[kLumenAudioFrequency2500Hz].value = 50;
	PatternHelpers.define_colors[kLumenAudioFrequency6250Hz].value = 50;
	PatternHelpers.define_colors[kLumenAudioFrequency16000Hz].value = 50;

}

void CLumen::seed_random() {
	uint8_t rand[4];
	uint8_t bytes_avail;
	do {
		sd_rand_application_bytes_available_get(&bytes_avail);
	} while(bytes_avail < 4);
	sd_rand_application_vector_get(&rand[0], 4);
	srand((int)&rand);
}

/*
 * LumenInterrupts.h
 *
 *  Created on: Jan 20, 2014
 *      Author: billy
 */

#ifndef LUMENINTERRUPTS_H
#define LUMENINTERRUPTS_H

#include "LumenAudio.h"

extern "C" {
	#include <stdbool.h>
	#include "nrf.h"
	#include "nrf_delay.h"
	#include "nrf_gpio.h"
	#include "nrf_soc.h"
	#include "app_timer.h"

	void TIMER1_IRQHandler(void);
	void TIMER2_IRQHandler(void);
	void ADC_IRQHandler(void);
	void RTC1_IRQHandler(void);
}

class CLumenInterrupts {
private:


	void stop_timer1(void);
	void start_timer1(void);

	void setup_timer1(void);
	void setup_timer2(void);
	void setup_rtc(void);

	void setup_adc(void);



public:
	volatile NRF_TIMER_Type * m_timer1 = NRF_TIMER1;
	volatile NRF_TIMER_Type * m_timer2 = NRF_TIMER2;

	volatile bool adc_is_active;
	bool schedule_config_save;
	bool schedule_config_load;
	volatile uint8_t freq_bands_counted = 7;
		volatile uint8_t freq_counter = 0;
		volatile uint16_t freq[kLumenAudioFrequencyCount];
		uint8_t strobe_clock = 2;
		volatile uint_fast16_t timer1_cc0 = 0;
		bool onSecondCount = false;
		int ignore_timer1;
		bool radio_is_starting = false;

	void init();
	void stop_timer2(void);
	void start_timer2(void);
	void start();
	void stop();



};

extern CLumenInterrupts LumenInterrupts;

#endif /* LUMENINTERRUPTS_H */

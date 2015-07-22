/*
 * LumenInterrupts.cpp
 *
 *  Created on: Jan 20, 2014
 *      Author: billy
 */

#include "LumenInterrupts.h"
#include "sun_board.h"
#include "BluetoothManager.h"
#include "TaskManager.h"
#include "TaskLeds.h"
#include "TaskPattern.h"
#include "TaskCapsense.h"
#include "LumenClock.h"
CLumenInterrupts LumenInterrupts;


// config for RTC (compare will trigger once a second)
#define LFCLK_FREQUENCY           (32768UL)                               /**< LFCLK frequency in Hertz, constant. */
#define RTC_FREQUENCY             (8UL)                                   /**< Required RTC working clock RTC_FREQUENCY Hertz. Changable. */
#define COMPARE_COUNTERTIME       (1UL)                                   /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */
#define COUNTER_PRESCALER         ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1)   /* f = LFCLK/(prescaler + 1) */



void CLumenInterrupts::init() {
	nrf_gpio_cfg_output(kSunBoardEQReset);
	nrf_gpio_pin_clear(kSunBoardEQReset);
	sd_clock_hfclk_request();
	uint32_t clock_is_running = 0;
	// wait for the 16MHz clock to start up.
	while (!clock_is_running) {
		sd_clock_hfclk_is_running(&clock_is_running);
	}



	this->setup_adc();
	this->setup_timer1();
	this->setup_rtc();
	this->setup_timer2();
}


void CLumenInterrupts::setup_rtc(void)
{
                                   
    NRF_RTC1->PRESCALER     = COUNTER_PRESCALER;                    // Set prescaler to a TICK of RTC_FREQUENCY.
    NRF_RTC1->CC[1]         = COMPARE_COUNTERTIME * RTC_FREQUENCY;  // Compare1 after approx COMPARE_COUNTERTIME seconds.

    // Enable COMPARE1 event and COMPARE1 interrupt:
    NRF_RTC1->EVTENSET 		= RTC_EVTENSET_TICK_Disabled;
    NRF_RTC1->EVTENSET      = RTC_EVTENSET_COMPARE1_Msk;
    NRF_RTC1->INTENSET      = RTC_INTENSET_COMPARE1_Msk;

    sd_nvic_ClearPendingIRQ(RTC1_IRQn);
    sd_nvic_SetPriority(RTC1_IRQn, 3);
    sd_nvic_EnableIRQ(RTC1_IRQn);
    NRF_RTC1->TASKS_START = 1;
}


void CLumenInterrupts::setup_timer1(void) {
	//Commented section for getting TIMER1_CC1 to work.
	uint_fast16_t volatile number_of_us = 50;
	timer1_cc0 = number_of_us;
	this->m_timer1->MODE = TIMER_MODE_MODE_Timer; // Set the timer in Timer Mode.
	this->m_timer1->PRESCALER = 4; // Prescaler 9 produces 31250 Hz timer frequency => 1 tick = 32 us.
	this->m_timer1->BITMODE = TIMER_BITMODE_BITMODE_32Bit; // 16 bit mode.
	this->m_timer1->TASKS_CLEAR = 1; // clear the task first to be usable for later.
	this->m_timer1->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
	//	this->m_timer1->INTENSET = TIMER_INTENSET_COMPARE1_Msk;
	//	this->m_timer1->SHORTS      = (TIMER_SHORTS_COMPARE1_CLEAR_Enabled << TIMER_SHORTS_COMPARE1_CLEAR_Pos);
	// With 32 us ticks, we need to multiply by 31.25 to get milliseconds.
	this->m_timer1->CC[0] = timer1_cc0;
	//	this->m_timer1->CC[1]          = 50000; //50ms
	sd_nvic_ClearPendingIRQ(TIMER1_IRQn);
	sd_nvic_SetPriority(TIMER1_IRQn, 3);
	sd_nvic_EnableIRQ(TIMER1_IRQn);
	this->m_timer1->TASKS_START = 1;
}

void CLumenInterrupts::setup_timer2(void) {
	uint_fast16_t volatile number_of_ms = 20;
	this->m_timer2->MODE = TIMER_MODE_MODE_Timer; // Set the timer in Timer Mode.
	this->m_timer2->PRESCALER = 9; // Prescaler 9 produces 31250 Hz timer frequency => 1 tick = 32 us.
	this->m_timer2->BITMODE = TIMER_BITMODE_BITMODE_32Bit; // 16 bit mode.
	this->m_timer2->TASKS_CLEAR = 1; // clear the task first to be usable for later.
	this->m_timer2->INTENSET = TIMER_INTENSET_COMPARE0_Msk;
	// With 32 us ticks, we need to multiply by 31.25 to get milliseconds.
	this->m_timer2->CC[0] = number_of_ms * 32.25;
	sd_nvic_ClearPendingIRQ(TIMER2_IRQn);
	sd_nvic_SetPriority(TIMER2_IRQn, 3);
	sd_nvic_EnableIRQ(TIMER2_IRQn);
	//this->m_timer2->TASKS_START = 1;                    // Start timer.
}

void CLumenInterrupts::stop_timer1(void) {
		this->ignore_timer1 = 7;
	this->radio_is_starting = true;
}

void CLumenInterrupts::start_timer1(void) {
#ifdef BOARD_SUN_1_1
		this->ignore_timer1 = 28;
	#endif
	#ifdef BOARD_SUN_1_3
		this->ignore_timer1 = 7;
	#endif
	#ifdef BOARD_SUN_1_3B
		this->ignore_timer1 = 7;
	#endif
	this->radio_is_starting = false;
}

void CLumenInterrupts::stop_timer2(void) {
	this->m_timer2->TASKS_CLEAR = 1;
	this->m_timer2->TASKS_STOP = 1;
}

void CLumenInterrupts::start_timer2(void) {
	this->m_timer2->TASKS_CLEAR = 1;
	this->m_timer2->TASKS_START = 1;
}

void CLumenInterrupts::start() {
	this->start_timer1();
	//this->start_timer2();
}

void CLumenInterrupts::stop() {
	this->stop_timer1();
	//this->stop_timer2();
}

void CLumenInterrupts::setup_adc(void) {

	// Configure ADC
	NRF_ADC->INTENSET = ADC_INTENSET_END_Msk;
	NRF_ADC->CONFIG = (ADC_CONFIG_RES_10bit << ADC_CONFIG_RES_Pos)
			| (ADC_CONFIG_INPSEL_AnalogInputOneThirdPrescaling
					<< ADC_CONFIG_INPSEL_Pos)
			| (ADC_CONFIG_REFSEL_SupplyOneThirdPrescaling
					<< ADC_CONFIG_REFSEL_Pos)
			| (kSunBoardEQDataADC << ADC_CONFIG_PSEL_Pos) |
			(ADC_CONFIG_EXTREFSEL_None << ADC_CONFIG_EXTREFSEL_Pos);
	NRF_ADC->EVENTS_END = 0;
	NRF_ADC->ENABLE = ADC_ENABLE_ENABLE_Enabled;

	// Enable ADC interrupt
	sd_nvic_ClearPendingIRQ(ADC_IRQn);
	sd_nvic_SetPriority(ADC_IRQn, 3);
	sd_nvic_EnableIRQ(ADC_IRQn);

	NRF_ADC->EVENTS_END = 0;    // Stop any running conversions.
}


/** @brief: Function for handling the RTC1 interrupts.
 * Triggered on COMPARE1 match.
 */
void RTC1_IRQHandler()
{
    if ((NRF_RTC1->EVENTS_COMPARE[1] != 0) && ((NRF_RTC1->INTENSET & RTC_INTENSET_COMPARE1_Msk) != 0)) {
    	LumenClock.on_tick();
		NRF_RTC1->EVENTS_COMPARE[1] = 0;
	//	NRF_RTC1->EVENTS_TICK       = 0;
	//	NRF_RTC1->EVENTS_OVRFLW     = 0;
    	NRF_RTC1->TASKS_CLEAR = 1;
    }
}


void TIMER1_IRQHandler(void) {

	if (LumenInterrupts.m_timer1->EVENTS_COMPARE[0] > 0) {
		LumenInterrupts.m_timer1->EVENTS_COMPARE[0] = 0;
		LumenInterrupts.strobe_clock++;
		if(LumenInterrupts.ignore_timer1 == 0 && !LumenInterrupts.radio_is_starting)
			LumenInterrupts.m_timer1->CC[0] = 50;
		//Wrap around clock
		if (LumenInterrupts.strobe_clock == 3)
			LumenInterrupts.strobe_clock = 0;
		//Strobe need to go high
		if (LumenInterrupts.strobe_clock == 0) {
			if(LumenInterrupts.ignore_timer1 > 0 && !LumenInterrupts.radio_is_starting)
				LumenInterrupts.m_timer1->CC[0] = 20;
			//unless its 7th freqband, then do a reset instead.
			if (LumenInterrupts.freq_bands_counted == kLumenAudioFrequencyCount) {
				LumenInterrupts.freq_counter = 0;
				nrf_gpio_pin_set(kSunBoardEQReset);
				nrf_delay_us(1);
				nrf_gpio_pin_clear(kSunBoardEQReset);
				nrf_gpio_pin_set(kSunBoardEQStrobe);
				LumenInterrupts.freq_bands_counted = 0;
			} else {
				nrf_gpio_pin_set(kSunBoardEQStrobe);
			}
		}
		//Strobe needs to go low
		else if (LumenInterrupts.strobe_clock == 1) {
			LumenInterrupts.m_timer1->CC[0] = 10;
			if(LumenInterrupts.ignore_timer1 > 0 && !LumenInterrupts.radio_is_starting)
				LumenInterrupts.m_timer1->CC[0] = 30;
			nrf_gpio_pin_clear(kSunBoardEQStrobe);
		}
		//Halfway through strobe low, start ADC.
		//(this is to get an accurate value according to EQ datasheet)
		else {
			LumenInterrupts.m_timer1->CC[0] = 90;
			LumenInterrupts.freq_bands_counted++;
			NRF_ADC->TASKS_START = 1;
		}
		//reset task
		LumenInterrupts.m_timer1->TASKS_CLEAR = 1;
	}

}
void TIMER2_IRQHandler(void) {

	if (LumenInterrupts.m_timer2->EVENTS_COMPARE[0] != 0) {

		//add events to scheduler (25ms since last pass)
	//	TaskManager.add_task(&TaskCapsense);
	//	TaskManager.add_task(&TaskPattern);
	//	TaskManager.add_task(&TaskLeds);

		BluetoothManager.flip_radio_bit();
		//clear timer2
		LumenInterrupts.m_timer2->EVENTS_COMPARE[0] = 0;
		LumenInterrupts.m_timer2->TASKS_CLEAR = 1;
		LumenInterrupts.stop_timer2();
	}

}

void ADC_IRQHandler(void) {

	LumenInterrupts.adc_is_active = true;
	if (NRF_ADC->EVENTS_END != 0) {
		//stop adc because our data is ready
		int32_t adc_input;
		NRF_ADC->EVENTS_END = 0;
		adc_input = NRF_ADC->RESULT;
		NRF_ADC->TASKS_STOP = 1;
		
		//should we ignore this data due to bluetooth rf interference?
		//If the radio is starting, we have a countdown until we need to quit reading adc values.
		//Once the radio has finished and !radio_is_starting, we need a countdown until we start reading the adc values.
		if ((!LumenInterrupts.radio_is_starting && LumenInterrupts.ignore_timer1 == 0 ) || (LumenInterrupts.radio_is_starting && LumenInterrupts.ignore_timer1 > 0)) {
			//we have good data, store it in our local buffer
			LumenInterrupts.freq[LumenInterrupts.freq_counter] = adc_input;
			if (LumenInterrupts.freq_counter == kLumenAudioFrequencyCount - 1) {

				//since we've collected a full 7-bucket sample, tell LumenAudio
				LumenAudio.new_audio_data(LumenInterrupts.freq);
			}
		}
		if (LumenInterrupts.ignore_timer1 > 0) {
			//decrement the timer1 timeout variable
			LumenInterrupts.ignore_timer1--;
		}
		//we collected one bucket, increment the counter
		LumenInterrupts.freq_counter++;

		//if we've collected all 7 buckets, reset and start at bucket 0
		if (LumenInterrupts.freq_counter == kLumenAudioFrequencyCount)
			LumenInterrupts.freq_counter = 0;
	}
	LumenInterrupts.adc_is_active = false;
}

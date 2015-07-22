/*
 * LumenAudio.cpp
 *
 *  Created on: Jan 22, 2014
 *      Author: billy
 */

#include "LumenAudio.h"
#include "Lumen.h"
CLumenAudio LumenAudio;

void CLumenAudio::init() {
	this->reset_max();


	//Scales each bucket to normalize their values.
	/* 20X*/
	sensitivity[kLumenAudioFrequency63Hz] = 50;//30
	sensitivity[kLumenAudioFrequency160Hz] = 30;//30
	sensitivity[kLumenAudioFrequency400Hz]= 80;//50
	sensitivity[kLumenAudioFrequency1000Hz] = 100;
	sensitivity[kLumenAudioFrequency2500Hz] = 80;
	sensitivity[kLumenAudioFrequency6250Hz] = 90;
	sensitivity[kLumenAudioFrequency16000Hz] = 100;
	/*13X
	sensitivity[kLumenAudioFrequency63Hz] = 20;
	sensitivity[kLumenAudioFrequency160Hz] = 20;
	sensitivity[kLumenAudioFrequency400Hz] = 100;
	sensitivity[kLumenAudioFrequency1000Hz] = 100;
	sensitivity[kLumenAudioFrequency2500Hz] = 100;
	sensitivity[kLumenAudioFrequency6250Hz] = 100;
	sensitivity[kLumenAudioFrequency16000Hz] = 100;
	*/


	/*20X*/
	//Each bucket's threshold
	/*threshholds[kLumenAudioFrequency63Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency63Hz]) / 100);
	threshholds[kLumenAudioFrequency160Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency160Hz]) / 100);
	threshholds[kLumenAudioFrequency400Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency400Hz]) / 100);
	threshholds[kLumenAudioFrequency1000Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency1000Hz]) / 100);
	threshholds[kLumenAudioFrequency2500Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency2500Hz]) / 100);
	threshholds[kLumenAudioFrequency6250Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency6250Hz]) / 100);
	threshholds[kLumenAudioFrequency16000Hz] = (uint16_t)((300 * (unsigned int)sensitivity[kLumenAudioFrequency16000Hz]) / 100);
	/**/

	//Each bucket's threshold
	threshholds[kLumenAudioFrequency63Hz] = (uint16_t)((135 * (unsigned int)sensitivity[kLumenAudioFrequency63Hz]) / 100);
	threshholds[kLumenAudioFrequency160Hz] = (uint16_t)((140 * (unsigned int)sensitivity[kLumenAudioFrequency160Hz]) / 100);
	threshholds[kLumenAudioFrequency400Hz] = (uint16_t)((125 * (unsigned int)sensitivity[kLumenAudioFrequency400Hz]) / 100);
	threshholds[kLumenAudioFrequency1000Hz] = (uint16_t)((125 * (unsigned int)sensitivity[kLumenAudioFrequency1000Hz]) / 100);
	threshholds[kLumenAudioFrequency2500Hz] = (uint16_t)((125 * (unsigned int)sensitivity[kLumenAudioFrequency2500Hz]) / 100);
	threshholds[kLumenAudioFrequency6250Hz] = (uint16_t)((120 * (unsigned int)sensitivity[kLumenAudioFrequency6250Hz]) / 100);
	threshholds[kLumenAudioFrequency16000Hz] = (uint16_t)((150 * (unsigned int)sensitivity[kLumenAudioFrequency16000Hz]) / 100);
	/**/

	#ifdef BOARD_SUN_1_1
	//Each bucket's threshold
	threshholds[kLumenAudioFrequency63Hz] = (uint16_t)((150 * (unsigned int)sensitivity[kLumenAudioFrequency63Hz]) / 100);
	threshholds[kLumenAudioFrequency160Hz] = (uint16_t)((90 * (unsigned int)sensitivity[kLumenAudioFrequency160Hz]) / 100);
	threshholds[kLumenAudioFrequency400Hz] = (uint16_t)((100 * (unsigned int)sensitivity[kLumenAudioFrequency400Hz]) / 100);
	threshholds[kLumenAudioFrequency1000Hz] = (uint16_t)((150 * (unsigned int)sensitivity[kLumenAudioFrequency1000Hz]) / 100);
	threshholds[kLumenAudioFrequency2500Hz] = (uint16_t)((120 * (unsigned int)sensitivity[kLumenAudioFrequency2500Hz]) / 100);
	threshholds[kLumenAudioFrequency6250Hz] = (uint16_t)((120 * (unsigned int)sensitivity[kLumenAudioFrequency6250Hz]) / 100);
	threshholds[kLumenAudioFrequency16000Hz] = (uint16_t)((120 * (unsigned int)sensitivity[kLumenAudioFrequency16000Hz]) / 100);
	#endif


}

void CLumenAudio::new_audio_data(volatile uint16_t* audio_data) {
	uint8_t buckets_above_thresh = 0;
	//the adc has collected a full set of data, lets process it
	for(int i=0; i<kLumenAudioFrequencyCount;i++) {
		//this->frequency_data[i] = (this->frequency_data[i] + 4  * audio_data[i]) / 5;

		this->frequency_data[i] = audio_data[i];


		this->normalized_frequency_data[i] = (uint16_t)(((unsigned int)frequency_data[i] * (unsigned int)sensitivity[i]) / 100);
		if (this->normalized_frequency_data[i] > this->max_frequency_data[i]) {
			this->max_frequency_data[i] = this->normalized_frequency_data[i];
		}
		if (this->normalized_frequency_data[i] > this->max_frequency_value) {
			this->max_frequency_value = this->normalized_frequency_data[i];
			this->max_frequency_position = i;
		}

		if (frequency_data[i] > 350) {
			buckets_above_thresh++;
			if (buckets_above_thresh >= 5)
				this->set_tap_detected(true);
		}

	}
}

void CLumenAudio::set_tap_detected(bool new_tap) {
	tap_detected = new_tap;
}

bool CLumenAudio::is_tap_detected() {
	return tap_detected;
}

uint16_t CLumenAudio::get_volume_bucket(int freq) {
	return this->frequency_data[freq];
}

uint16_t CLumenAudio::get_normalized_volume_bucket(int freq) {
	return this->normalized_frequency_data[freq];
}

uint16_t CLumenAudio::get_max_volume_bucket(int freq) {
	return this->max_frequency_data[freq];
}


uint16_t CLumenAudio::get_max_value(void) {
	return this->max_frequency_value;
}

uint16_t CLumenAudio::get_max_position(void) {
	return this->max_frequency_position;
}

void CLumenAudio::reset_max(void) {
	for(int i=0; i<kLumenAudioFrequencyCount;i++) {
		this->max_frequency_data[i] = 0;
		}
	this->max_frequency_value = 0;
}


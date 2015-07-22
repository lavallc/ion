/*
 * LumenAudio.h
 *
 *  Created on: Jan 22, 2014
 *      Author: billy
 */

#ifndef LUMENAUDIO_H_
#define LUMENAUDIO_H_

#include <cstdint>

enum kLumenAudioFrequency {
	kLumenAudioFrequency63Hz = 0,
	kLumenAudioFrequency160Hz,
	kLumenAudioFrequency400Hz,
	kLumenAudioFrequency1000Hz,
	kLumenAudioFrequency2500Hz,
	kLumenAudioFrequency6250Hz,
	kLumenAudioFrequency16000Hz,
	kLumenAudioFrequencyCount,
};

typedef struct __attribute__ ((packed)){
	int hue;
	int saturation;
	int value;
} define_color_t;

class CLumenAudio {
private:
	uint16_t frequency_data[kLumenAudioFrequencyCount];
	uint16_t normalized_frequency_data[kLumenAudioFrequencyCount];
	uint16_t max_frequency_data[kLumenAudioFrequencyCount]; //Based off of normalized data
	uint16_t max_frequency_value;
	int max_frequency_position;

	bool tap_detected = false;
public:
	void set_tap_detected(bool new_tap);
	bool is_tap_detected(void);
	uint16_t threshholds[kLumenAudioFrequencyCount];
	uint16_t sensitivity[kLumenAudioFrequencyCount];
	void init();
	void new_audio_data(volatile uint16_t* audio_data);
	void reset_max(void);
	uint16_t get_volume_bucket(int freq);
	uint16_t get_max_volume_bucket(int freq);
	uint16_t get_normalized_volume_bucket(int freq);
	uint16_t get_max_value(void);
	uint16_t get_max_position(void);

};

extern CLumenAudio LumenAudio;

#endif /* LUMENAUDIO_H_ */

/*
 * TaskCapsense.cpp
 *
 *  Created on: Jan 23, 2014
 *      Author: billy
 */

#include "TaskCapsense.h"
#include "Lumen.h"
#include "LumenAudio.h"
#include "PatternManager.h"
#include "LumenSettings.h"
#include "PatternOff.h"

CTaskCapsense TaskCapsense;


void CTaskCapsense::run() {
	if(Lumen.capsense->is_touched()) {
		prox_touch_on = false;
		if(!capsense_touch_dispatched_already) {
			//dispatch touch event

			for(LumenCapsenseEvent evt : Lumen.capsense->event_handlers) {
				evt();
			}

			capsense_touch_dispatched_already = true;
		}
		else {
			capsense_held_time++;
			if (capsense_held_time == 80) {
				PatternManager.set_current_pattern(1);
				if (BluetoothManager.device_is_connected())
					PacketFactory.writeMood(kLumenRequestIdFromLamp, kLumenPacketOpCodeGetCurrentMood, 1);
				capsense_touch_dispatched_already = true;
			}
		}
	}else {
		if (LumenSettings.is_knock_enabled()) {
			//Min time to turn back on/off
			if (prox_touch_wait < 60)
				prox_touch_wait++;
			//Start counting after first tap.
			if (prox_touch_wait > 60) {
				uint8_t low_volume = 0;
				for (int i = 0; i < kLumenAudioFrequencyCount; i++)
					if (LumenAudio.get_volume_bucket(i) < 100) {
						low_volume++;
				}
				if (low_volume == 7)
					knock_ended = true;
				prox_touch_wait++;
			}
			//Time after first tap to listen for second tap.
			if (prox_touch_wait == 100) {
				knock_ended = false;
				prox_touch_wait = 0;
			}
			if (LumenAudio.is_tap_detected()) {
				LumenAudio.set_tap_detected(false);
				//A double tap can only occur if the current pattern is OFF
				if (PatternManager.is_pattern_off() && !prox_touch_on && prox_touch_wait == 60) {
					prox_touch_wait = 61;

				}
				//Detected a double tap while pattern is still OFF
				else if (PatternManager.is_pattern_off() && !prox_touch_on && prox_touch_wait > 70 && knock_ended) {
					prox_touch_on = true;
					prox_touch_wait = 0;
					knock_ended = false;
					PatternOff.handle_config_data(1, 1);
				}
				//If a double taps turns on ION, then a double tap can turn if off.
				//ONLY if noone uses capsense
				else if (prox_touch_on == true && prox_touch_wait == 60) {
					prox_touch_wait = 61;
				}
				//Detected a double tap, so turn off.
				else if (prox_touch_on == true && prox_touch_wait > 70 && knock_ended) {
					prox_touch_on = false;
					prox_touch_wait = 0;
					PatternOff.handle_config_data(1, 0);
					knock_ended = false;
				}
			}
		}
		capsense_touch_dispatched_already = false;
		capsense_held_time = 0;
	}
}

bool CTaskCapsense::is_repeating() {
	return false;
}

/*
 * Serial.h
 *
 *  Created on: Jan 30, 2014
 *      Author: billy
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <string>
#include <cstdint>
extern "C" {
	#include "nrf.h"
	#include "nrf_delay.h"
	#include "nrf_gpio.h"
}
#define BUFFER_SIZE 100

using namespace std;

class CSerial {
private:
	uint8_t uart_get(void);
	bool uart_get_with_timeout(int32_t timeout_ms, uint8_t *rx_data);

	void put(char cr);
	void putstring(const char *str);
	uint8_t buffer[BUFFER_SIZE];
	int buffer_start = 0;
	int buffer_end = 0;

public:
	int store_in_buffer(uint8_t * data, int length);
	void debug_print(string statement);
	void debug_print(int number);
	void print_from_buffer();
	void begin(int baudrate, int tx_pin, int rx_pin);
	void print(string s);
	void print(int s);
	void print(uint32_t s);

	void print(uint8_t s);
	void print(float s);

	void println(string s);
};

extern CSerial Serial;
#endif /* SERIAL_H_ */

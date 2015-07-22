/*
 * Serial.cpp
 *
 *  Created on: Jan 30, 2014
 *      Author: billy
 */

#include "Serial.h"
#include "stdio.h"
#include <stdint.h>

CSerial Serial;

uint8_t CSerial::uart_get(void)
{
  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    // Wait for RXD data to be received
  }

  NRF_UART0->EVENTS_RXDRDY = 0;
  return (uint8_t)NRF_UART0->RXD;
}

bool CSerial::uart_get_with_timeout(int32_t timeout_ms, uint8_t *rx_data)
{
  bool ret = true;

  while (NRF_UART0->EVENTS_RXDRDY != 1)
  {
    if (timeout_ms-- >= 0)
    {
      // wait in 1ms chunk before checking for status
      nrf_delay_us(1000);
    }
    else
    {
      ret = false;
      break;
    }
  }  // Wait for RXD data to be received

  if (timeout_ms >= 0)
  {
    // clear the event and set rx_data with received byte
      NRF_UART0->EVENTS_RXDRDY = 0;
      *rx_data = (uint8_t)NRF_UART0->RXD;
  }

  return ret;
}

void CSerial::put(char cr)
{
  NRF_UART0->TXD = (uint8_t)cr;

  while (NRF_UART0->EVENTS_TXDRDY!=1)
  {
    // Wait for TXD data to be sent
  }

  NRF_UART0->EVENTS_TXDRDY=0;
}

void CSerial::putstring(const char *str)
{
  uint_fast8_t i = 0;
  uint8_t ch = str[i++];
  while (ch != '\0')
  {
    this->put(ch);
    ch = str[i++];
  }
}

void CSerial::begin(int baudrate, int tx_pin, int rx_pin) {
	//configure gpio directions
	nrf_gpio_cfg_output(tx_pin);
	nrf_gpio_cfg_input(rx_pin, NRF_GPIO_PIN_NOPULL);

	//tell UART0 about pins
	NRF_UART0->PSELTXD = tx_pin;
	NRF_UART0->PSELRXD = rx_pin;

	//figure out baudrate
	uint32_t baudmask = UART_BAUDRATE_BAUDRATE_Baud115200;
	if(baudrate == 1200) 		baudmask = UART_BAUDRATE_BAUDRATE_Baud1200;
	else if(baudrate == 2400) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud2400;
	else if(baudrate == 4800) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud4800;
	else if(baudrate == 9600) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud9600;
	else if(baudrate == 14400) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud14400;
	else if(baudrate == 19200) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud19200;
	else if(baudrate == 28800) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud28800;
	else if(baudrate == 38400) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud38400;
	else if(baudrate == 57600) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud57600;
	else if(baudrate == 76800) 	baudmask = UART_BAUDRATE_BAUDRATE_Baud76800;
	else if(baudrate == 115200) baudmask = UART_BAUDRATE_BAUDRATE_Baud115200;
	else if(baudrate == 230400) baudmask = UART_BAUDRATE_BAUDRATE_Baud230400;
	else if(baudrate == 250000) baudmask = UART_BAUDRATE_BAUDRATE_Baud250000;
	else if(baudrate == 460800) baudmask = UART_BAUDRATE_BAUDRATE_Baud460800;
	else if(baudrate == 460800) baudmask = UART_BAUDRATE_BAUDRATE_Baud460800;
	else if(baudrate == 921600) baudmask = UART_BAUDRATE_BAUDRATE_Baud921600;
	else if(baudrate == 1000000) baudmask = UART_BAUDRATE_BAUDRATE_Baud1M;

	//configure uart0 option registers
	NRF_UART0->BAUDRATE         = (baudmask << UART_BAUDRATE_BAUDRATE_Pos);
	NRF_UART0->ENABLE           = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
	NRF_UART0->TASKS_STARTTX    = 1;
	NRF_UART0->TASKS_STARTRX    = 1;
	NRF_UART0->EVENTS_RXDRDY    = 0;

}

void CSerial::print(string s) {
	this->putstring(s.c_str());
}

void CSerial::debug_print(string statement) {
	uint8_t temp_buffer[BUFFER_SIZE/2];
	sprintf((char *)temp_buffer, "%s" ,statement.c_str());
	store_in_buffer(temp_buffer,statement.length());
}

void CSerial::debug_print(int number) {
	char number_str[BUFFER_SIZE/2];
	int length = 0;
	sprintf(number_str, "%x", number);
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
		if (number_str[i] != '\0')
			length++;
		else
			break;
	}
	store_in_buffer((uint8_t *)number_str,length);
}

int CSerial::store_in_buffer(uint8_t * data, int length) {
	if (buffer_end + length + 2 < BUFFER_SIZE) {
		buffer[buffer_end] = '\n';
		buffer[buffer_end+1] = '\r';
		buffer_end += 2;
		for (int i = buffer_end; i < buffer_end + length; i++) {
			buffer[i] = data[i- buffer_end];
		}
		buffer_end += length;
		buffer[buffer_end] = '\0';
	}
	else
		return 1;

}

void CSerial::print_from_buffer() {
	this->putstring((char *)buffer);
	buffer_start = 0;
	buffer_end = 0;
	buffer[0] = '\0';
}

void CSerial::print(int s) {
	char buf[50];
	sprintf(buf, "%i", s);
	this->putstring(buf);
}

void CSerial::print(uint32_t s) {
	char buf[50];
	sprintf(buf, "%u", s);
	this->putstring(buf);
}


void CSerial::print(uint8_t s) {
	char buf[50];
	sprintf(buf, "%02x", s);
	this->putstring(buf);
}

void CSerial::print(float s) {
	char buf[50];
	sprintf(buf, "%f", s);
	this->putstring(buf);
}


void CSerial::println(string s) {
	this->putstring(s.c_str());
	this->put('\r');
	this->put('\n');
}

/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "spi.h"

// For a quick test
#ifndef F_CPU
#define F_CPU 1000000UL
#endif
#include <util/delay.h>

static uart_message_element_t received_data;

static char* restart_message_format 					= "\n\rHello World.\n\r";
static char* spi_initialized_message_format 	= "SPI perhipheral initialized.\n\r";
static char* spi_echo_message_format 					= "Received message over SPI: %02x%02x%02x%02x\n\r";

// "DEADBEEF", an easily recognizeable hex number.
static spi_message_element_t spi_message[] = {0xDE, 0xAD, 0xBE, 0xEF};
static int spi_message_length = 4;

void echo_spi_received(
	const spi_message_element_t *received_message,
	int received_message_length
);

int main() {

	// Let's wait for all of our hardware to turn on.
	_delay_ms(1000);

	uart_initialize();

	while(1) {
		uart_transmit_formatted_message(restart_message_format);
		_delay_ms(1000);
	}
	
	// Wait until the message is done being transmitted.
	while((UCSR0B & _BV(UDRIE0)) != 0);
	// TODO: Make a macro or function to replace this?

	spi_initialize();
	uart_transmit_formatted_message(spi_initialized_message_format);

	// Wait until the message is done being transmitted.
	while((UCSR0B & _BV(UDRIE0)) != 0);
	// TODO: Make a macro or function to replace this?

	// SPDR = 0xAA;

	
	spi_begin_transaction(
		spi_message,
		spi_message_length,
		echo_spi_received
	);
	

	while(1);

}

void echo_spi_received(
	const spi_message_element_t *received_message,
	int received_message_length
) {

	uart_transmit_formatted_message(
		spi_echo_message_format,
		received_message[0],
		received_message[1],
		received_message[2],
		received_message[3]
	);

	// Wait until the message is done being transmitted.
	//while((UCSR0B & _BV(UDRIE0)) != 0);
	// TODO: Make a macro or function to replace this?

}

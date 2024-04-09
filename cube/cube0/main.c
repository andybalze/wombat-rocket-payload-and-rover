/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "spi.h"

#define F_CPU 1000000
#include <util/delay.h>

#define STARTUP_DELAY_MS (100)

static char* restart_message_format 	= "\n\rHello World.\n\r";
static char* spi_transmitting_format	= "Transmitting message over SPI:\t%s\n\r";
static char* spi_received_format			= "Received message over SPI:\t%s\n\r";

static spi_message_element_t* spi_message = "Praise the Omnissiah!";
int spi_message_length = 21;

int main() {

	// Let's wait for all of our hardware to turn on.
	_delay_ms(STARTUP_DELAY_MS);

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	spi_message_element_t response[SPI_TRANSACTION_MAX_LENGTH];
	spi_initialize();
	uart_transmit_formatted_message(spi_transmitting_format, spi_message);
	UART_WAIT_UNTIL_DONE();
	spi_execute_transaction(response, 1, spi_message, spi_message_length);
	response[spi_message_length] = '\0';
	uart_transmit_formatted_message(spi_received_format, response);
	UART_WAIT_UNTIL_DONE();

	while(1);

}

/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "spi.h"
#include "trx.h"

static uart_message_element_t received_data;

static char* restart_message_format = "\n\rHello World.\n\r";
static char* status_message_format = "Transceiver has a status of %x.\n\r";

int main() {

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	
	// Wait until the message is done being transmitted.
	while((UCSR0B & _BV(UDRIE0)) != 0);
	// TODO: Make a macro or function to replace this?

	trx_initialize();
	// As part of this function, the status is retrieved from the transceiver.
	
	// Wait until the SPI transaction finishes.
	while((SPCR & _BV(SPIE)) != 0);

	spi_message_element_t status = trx_get_status();
	uart_transmit_formatted_message(status_message_format, status);

	while(1);

}

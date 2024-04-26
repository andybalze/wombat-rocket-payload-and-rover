/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "digital_io.h"
#include "uart.h"
#include "spi.h"
#include "trx.h"

#define F_CPU 1000000
#include <util/delay.h>

#define STARTUP_DELAY_MS (100)

#define THIS_CUBE_RX_ADDRESS 0x0A0A0A0A
#define TARGET_RX_ADDRESS THIS_CUBE_RX_ADDRESS

static char* restart_message_format 	= "\n\rHello World.\n\r";
static char* received_message_format 	= "Received message:\t%s\n\r";

static trx_payload_element_t *example_payload1 = "This better work, cuz if not...";
static trx_payload_element_t *example_payload2 = "I swear I'll shit yourself!!!\n\r";

static trx_payload_element_t received_payload[TRX_PAYLOAD_LENGTH + 1];

int main() {

	// Let's wait for all of our hardware to turn on.
	_delay_ms(STARTUP_DELAY_MS);

	digital_io_initialize();
	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	
	trx_initialize(THIS_CUBE_RX_ADDRESS);

	trx_transmission_outcome_t outcome;

	uart_transmit_formatted_message("Attempting to transmit payload 1.\n\r");
	UART_WAIT_UNTIL_DONE();
	if (trx_transmit_payload(TARGET_RX_ADDRESS, example_payload1, TRX_PAYLOAD_LENGTH) == TRX_TRANSMISSION_SUCCESS) {
		uart_transmit_formatted_message("Successfully transmitted payload 1.\n\r");
	} else {
		uart_transmit_formatted_message("Failed to transmit payload 1.\n\r");
	}
	UART_WAIT_UNTIL_DONE();

	_delay_ms(1000);
	
	uart_transmit_formatted_message("Attempting to transmit payload 2.\n\r");
	UART_WAIT_UNTIL_DONE();
	if (trx_transmit_payload(TARGET_RX_ADDRESS, example_payload2, TRX_PAYLOAD_LENGTH) == TRX_TRANSMISSION_SUCCESS) {
		uart_transmit_formatted_message("Successfully transmitted payload 2.\n\r");
	} else {
		uart_transmit_formatted_message("Failed to transmit payload 2.\n\r");
	}
	UART_WAIT_UNTIL_DONE();

	while (1) {

		trx_receive_payload(received_payload, TRX_TIMEOUT_INDEFINITE);
		uart_transmit_formatted_message(received_message_format, received_payload);
		UART_WAIT_UNTIL_DONE();

	}

	while(1);

}

/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

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

static trx_payload_element_t received_payload[TRX_PAYLOAD_LENGTH];

int main() {

	// Let's wait for all of our hardware to turn on.
	_delay_ms(STARTUP_DELAY_MS);

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	
	trx_initialize(THIS_CUBE_RX_ADDRESS);
	/*
	trx_transmit_payload(TARGET_RX_ADDRESS, example_payload1, TRX_PAYLOAD_LENGTH);
	trx_transmit_payload(TARGET_RX_ADDRESS, example_payload2, TRX_PAYLOAD_LENGTH);

	while (1) {

		trx_receive_payload(received_payload);
		uart_transmit_formatted_message(received_message_format, received_payload);
		UART_WAIT_UNTIL_DONE();

	}
	*/

	while(1);

}

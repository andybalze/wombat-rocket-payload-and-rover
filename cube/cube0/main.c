////////////////////////////////////////////////////////////////////////////////
//
// Reliability Test: Transmitter
//
// This main function is one half of a setup for testing the reliability of data
// links under different conditions. This program attempts to transmit a number
// of messages, and keeps track of how many succeed. In this situation, success
// is measured when a packet is acknowledged by the receiver.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include "address.h"

#include "uart.h"
#include "spi.h"
#include "trx.h"

#define F_CPU 1000000
#include <util/delay.h>

#define STARTUP_DELAY_MS 			(100)
#define TRANSMISSION_DELAY_MS	(1000)

#define TRANSMISSION_COUNT		(100)

#define RECEIVER_DATA_LINK_ADDR (0x0B0B0B0B)

static char* restart_message_format 		= "\n\rHello World.\n\r";
static char* begin_test_message_format	= "Beginning test run of %u transmission.\n\r";
static char* report_message_format			= "Finished test. %u/%u transmission were received and acknowledged.\n\r";

static char* example_payload = "It hard to write exactly 32 cha";

int main() {

	// Let's wait for all of our hardware to turn on.
	_delay_ms(STARTUP_DELAY_MS);

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	trx_initialize(MY_DATA_LINK_ADDR);
	uart_transmit_formatted_message(begin_test_message_format, TRANSMISSION_COUNT);
	UART_WAIT_UNTIL_DONE();

	trx_transmission_outcome_t outcome;
	int successful_transmissions = 0;
	for (int i = 0; i < TRANSMISSION_COUNT; i++) {
		_delay_ms(TRANSMISSION_DELAY_MS);
		outcome = trx_transmit_payload(RECEIVER_DATA_LINK_ADDR, example_payload, TRX_PAYLOAD_LENGTH);
		if (outcome) {
			successful_transmissions = successful_transmissions + 1;
		}
	}

	uart_transmit_formatted_message(report_message_format, successful_transmissions, TRANSMISSION_COUNT);

	while(1);

}

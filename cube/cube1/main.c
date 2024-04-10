////////////////////////////////////////////////////////////////////////////////
//
// Reliability Test: Receiver
//
// This main function is one half of a setup for testing the reliability of data
// links under different conditions. This program attempts to receive an
// unlimited number of transmissions. The cumulative number of transmissions it
// has received is reported after each transmission.
//
////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "address.h"

#include "uart.h"
#include "spi.h"
#include "trx.h"

#define F_CPU 1000000
#include <util/delay.h>

#define STARTUP_DELAY_MS 			(100)

static char* restart_message_format 		= "\n\rHello World.\n\r";
static char* begin_test_message_format	= "Listening for transmissions.\n\r";
static char* report_message_format			= "Received transmission. Cumulative transmission count: %u\n\r";

int main() {

	// Let's wait for all of our hardware to turn on.
	_delay_ms(STARTUP_DELAY_MS);

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	trx_initialize(MY_DATA_LINK_ADDR);
	uart_transmit_formatted_message(begin_test_message_format);
	UART_WAIT_UNTIL_DONE();

	int transmission_count = 0;
	while(1) {
		trx_receive_payload(NULL);
		transmission_count = transmission_count + 1;
		uart_transmit_formatted_message(report_message_format, transmission_count);
		UART_WAIT_UNTIL_DONE();
	}

}

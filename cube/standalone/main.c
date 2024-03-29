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
static char* status_message_format = "Transceiver has a status of %02x.\n\r";

int main() {

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	trx_initialize();

	while(1);

}

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
static spi_message_element_t *example_payload = "I hope this works, cuz if not...";
static trx_address_t address_transmit_to = 0x600DD00D;

int main() {

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	trx_initialize();
	trx_transmit_payload(
		address_transmit_to, 
		example_payload, 
		TRX_PAYLOAD_LENGTH
	);

	while(1);

}

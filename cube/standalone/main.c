/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"
#include "spi.h"
#include "trx.h"

#define THIS_RX_ADDRESS 0xDEADBEEF
#define TARGET_ADDRESS	0x600DD00D

static uart_message_element_t received_data;

static char* restart_message_format = "\n\rHello World.\n\r";
static char* status_message_format = "Transceiver has a status of %02x.\n\r";
static spi_message_element_t *example_payload = "I hope this works, cuz if not...";

int main() {

	uart_initialize();
	uart_transmit_formatted_message(restart_message_format);
	UART_WAIT_UNTIL_DONE();

	trx_initialize(THIS_RX_ADDRESS);
	trx_transmit_payload(
		TARGET_ADDRESS, 
		example_payload, 
		TRX_PAYLOAD_LENGTH
	);



	while(1);

}

/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart.h"

static uart_message_element_t received_data;

static char* restart_message = "Hello World.\n\r";
static int restart_message_length = 14;

int main() {

	uart_initialize();

	uart_transmit_message(restart_message, restart_message_length);

	while(1);

}

// Echo any message received.
ISR(USART_RX_vect) {
	received_data = UDR0;
	uart_transmit_message(&received_data, 1);
}

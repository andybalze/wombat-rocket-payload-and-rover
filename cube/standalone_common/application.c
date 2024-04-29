#include "application.h"
#include "transport.h"
#include "networking_constants.h"
#include "uart.h"
#include "address.h"
#include "digital_io.h"
#include "log.h"
#include "trx.h"
#include "network.h"

#include <stdio.h>
#include <string.h>

#include "cube_parameters.h"
#include <util/delay.h>

void application() {

    // To save on memory, the same buffer is used to store a received message
    // and to prepare a message to transmit.
    char message[MAX_SEGMENT_LEN];
    uint16_t message_len;
    byte who_sent_me_this;

    uint16_t num_messages_this_session = 0;

    uart_transmit_formatted_message("::: Data cube %02x activated. Entering network mode. :::\r\n", MY_NETWORK_ADDR);
    UART_WAIT_UNTIL_DONE();

    LED_set(LED_WHITE);

    while(true) {
        network_rx(message, MAX_SEGMENT_LEN, TRX_TIMEOUT_INDEFINITE);
        _delay_ms(400);
    }
}

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

#define ROVER_APP_DELAY_BETWEEN_MSG_MS (2000UL)
#define ROVER_APP_RESPONSE_TIMEOUT (3000UL)

void application() {

    // To save on memory, the same buffer is used to store a received message
    // and to prepare a message to transmit.
    char message[MAX_SEGMENT_LEN];
    uint16_t message_len;
    byte who_sent_me_this;

    uint16_t num_messages_this_session = 0;

    uart_transmit_formatted_message("::: Rover's transceiver activated. Entering network mode. :::\r\n");
    UART_WAIT_UNTIL_DONE();

    LED_set(LED_WHITE);

    while(true) {
        snprintf(message, MAX_SEGMENT_LEN, "Test packet\r\n");
        network_tx(message, 15, 0x3c, MY_NETWORK_ADDR);
        _delay_ms(2000);
    }
}

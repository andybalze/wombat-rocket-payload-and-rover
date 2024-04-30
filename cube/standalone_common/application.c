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
    char message[MAX_MESSAGE_LEN];
    uint16_t message_len;
    byte who_sent_me_this;

    transport_rx_result result;

    uint16_t num_messages_this_session = 0;

    uart_transmit_formatted_message("::: Data cube %02x activated. Entering network mode. :::\r\n", MY_NETWORK_ADDR);
    UART_WAIT_UNTIL_DONE();

    LED_set(LED_WHITE);

    while(true) {
        result = transport_rx(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this, TRX_TIMEOUT_INDEFINITE);
        if (result == TRANSPORT_RX_SUCCESS) {
            message[MAX_MESSAGE_LEN - 1] = 0;
            uart_transmit_formatted_message("Got the following message: %s\r\n", message);
            UART_WAIT_UNTIL_DONE();
        }
        if (result == TRANSPORT_RX_TIMEOUT) {
            uart_transmit_formatted_message("[WARNING] Transport layer RX timed out %s\r\n", message);
            UART_WAIT_UNTIL_DONE();
        }
        if (result == TRANSPORT_RX_ERROR) {
            uart_transmit_formatted_message("[WARNING] Transport layer RX encountered an error %s\r\n", message);
            UART_WAIT_UNTIL_DONE();
        }
    }
}

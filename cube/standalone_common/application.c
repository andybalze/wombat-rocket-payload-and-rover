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

// read message and adjust the LED accordingly
void parse_message(char* message) {

    char search[12];

    snprintf(search, sizeof(search), "LED:OFF");
    if (strstr(message, search) != NULL) {
        LED_set(LED_OFF);
        return;
    }

    snprintf(search, sizeof(search), "LED:BLUE");
    if (strstr(message, search) != NULL) {
        LED_set(LED_BLUE);
        return;
    }

    snprintf(search, sizeof(search), "LED:GREEN");
    if (strstr(message, search) != NULL) {
        LED_set(LED_GREEN);
        return;
    }

    snprintf(search, sizeof(search), "LED:CYAN");
    if (strstr(message, search) != NULL) {
        LED_set(LED_CYAN);
        return;
    }

    snprintf(search, sizeof(search), "LED:RED");
    if (strstr(message, search) != NULL) {
        LED_set(LED_RED);
        return;
    }

    snprintf(search, sizeof(search), "LED:MAGENTA");
    if (strstr(message, search) != NULL) {
        LED_set(LED_MAGENTA);
        return;
    }

    snprintf(search, sizeof(search), "LED:YELLOW");
    if (strstr(message, search) != NULL) {
        LED_set(LED_YELLOW);
        return;
    }

    snprintf(search, sizeof(search), "LED:WHITE");
    if (strstr(message, search) != NULL) {
        LED_set(LED_WHITE);
        return;
    }

    return;
}

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

    LED_set(LED_BLUE);

    while(true) {

        /*
        network_rx(message, MAX_SEGMENT_LEN, TRX_TIMEOUT_INDEFINITE);
        */

        result = transport_rx(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this, TRX_TIMEOUT_INDEFINITE);
        if (result == TRANSPORT_RX_SUCCESS) {
            message[MAX_MESSAGE_LEN - 1] = 0;
            parse_message(message);
            uart_transmit_formatted_message("=== Got a message ===\r\n%s\r\n=====================\r\n", message);
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

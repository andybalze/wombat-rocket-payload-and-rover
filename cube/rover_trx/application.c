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
        LED_update_color(LED_OFF);
        return;
    }

    snprintf(search, sizeof(search), "LED:BLUE");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_BLUE);
        return;
    }

    snprintf(search, sizeof(search), "LED:GREEN");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_GREEN);
        return;
    }

    snprintf(search, sizeof(search), "LED:CYAN");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_CYAN);
        return;
    }

    snprintf(search, sizeof(search), "LED:RED");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_RED);
        return;
    }

    snprintf(search, sizeof(search), "LED:MAGENTA");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_MAGENTA);
        return;
    }

    snprintf(search, sizeof(search), "LED:YELLOW");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_YELLOW);
        return;
    }

    snprintf(search, sizeof(search), "LED:WHITE");
    if (strstr(message, search) != NULL) {
        LED_update_color(LED_WHITE);
        return;
    }

    return;
}


// call transport_tx and handle the error messages.
void application_tx(byte* message, uint16_t message_len, byte dest_port) {
    transport_tx_result result;
    result = transport_tx(message, 79, 0x3c);
    if (result == TRANSPORT_TX_REACHED_ATTEMPT_LIMIT) {
        uart_transmit_formatted_message("[WARNING] Transport layer reached attempt limit\r\n");
        UART_WAIT_UNTIL_DONE();
    }
    if (result == TRANSPORT_TX_ERROR) {
        uart_transmit_formatted_message("[WARNING] Transport layer encountered an error\r\n");
        UART_WAIT_UNTIL_DONE();
    }
    return;
}

void application() {

    // To save on memory, the same buffer is used to store a received message
    // and to prepare a message to transmit.
    char message[MAX_MESSAGE_LEN];
    uint16_t message_len;
    byte who_sent_me_this;

    uint16_t num_messages_this_session = 0;

    uart_transmit_formatted_message("::: Rover's transceiver activated. Entering network mode. :::\r\n");
    UART_WAIT_UNTIL_DONE();

    LED_update_color(LED_BLUE);

    while(true) {

        snprintf(message, MAX_MESSAGE_LEN, "Everything should be made as simple as possible,\r\nbut no simpler.\r\nLED:GREEN\r\n");
        application_tx(message, 79, 0x3c);
        _delay_ms(5000);

        snprintf(message, MAX_MESSAGE_LEN, "Please work. I will buy you a coffee.\r\nLED:MAGENTA\r\n");
        application_tx(message, 53, 0x3c);
        _delay_ms(5000);
    }
}

#include "application.h"
#include "transport.h"
#include "networking_constants.h"
#include "uart.h"
#include "address.h"
#include "digital_io.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

// note: currently only used for debug
#include "cube_parameters.h"
#include <util/delay.h>

// read message and adjust the LED accordingly
void parse_message(char* message) {

    char search[12];

    strncpy(search, "LED:OFF", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_OFF);
        return;
    }

    strncpy(search, "LED:BLUE", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_BLUE);
        return;
    }

    strncpy(search, "LED:GREEN", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_GREEN);
        return;
    }

    strncpy(search, "LED:CYAN", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_CYAN);
        return;
    }

    strncpy(search, "LED:RED", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_RED);
        return;
    }

    strncpy(search, "LED:MAGENTA", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_MAGENTA);
        return;
    }

    strncpy(search, "LED:YELLOW", sizeof(search));
    if (strstr(message, search) != NULL) {
        LED_set(LED_YELLOW);
        return;
    }

    strncpy(search, "LED:WHITE", sizeof(search));
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

    uint16_t num_messages_this_session = 0;

    uart_transmit_formatted_message("::: Data cube activated. Entering network mode. :::\r\n");
    UART_WAIT_UNTIL_DONE();

    LED_set(LED_WHITE);

    while(true) {

        // listen for a message
        for (int i = 0; i < MAX_MESSAGE_LEN; i++) message[i] = 0;
        transport_rx(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this);
        num_messages_this_session++;

        // force the string to be null-terminated if it isn't already
        message[MAX_MESSAGE_LEN - 1] = '\0';

        // report that the message was received
        uart_transmit_formatted_message("========== Received message from %02x ==========\r\n", who_sent_me_this);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message(message);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("================================================\r\n");
        UART_WAIT_UNTIL_DONE();

        // record it
        log_message(message, message_len, who_sent_me_this);        

        // parse the message and light the LED depending on the result
        parse_message(message);

        // compose the response
        for (int i = 0; i < MAX_MESSAGE_LEN; i++) message[i] = 0;
        snprintf(message, MAX_MESSAGE_LEN, "Hello, whoever lives at address %02x. I am cube %02x.\r\nI have received %d messages since powering on.\r\nThanks for reaching out.\r\n", who_sent_me_this, MY_PORT, num_messages_this_session);

        // transmit the response
        // note: the +1 is to include the null terminator in the message
        transport_tx(message, strlen(message) + 1, who_sent_me_this);

        // DEBUG! Remove this after you've determined the EEPROM isn't being written to a whole bunch.
        _delay_ms(1000);

    }
}

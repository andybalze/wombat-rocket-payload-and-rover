#include "application.h"
#include "transport.h"
#include "networking_constants.h"
#include "uart.h"
#include "address.h"
#include "digital_io.h"
#include "log.h"
#include "trx.h"

#include <stdio.h>
#include <string.h>

// note: currently only used for debug
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

// Same as transport_rx, with the side effects of
// logging the result and printing the message to UART.
bool listen(byte* buffer, uint16_t buf_len, uint16_t* message_len, byte* src, uint16_t timeout_ms) {

    uart_transmit_formatted_message("Listening for a message... ");
    UART_WAIT_UNTIL_DONE();

    for (int i = 0; i < buf_len; i++) buffer[i] = 0;
    bool success = transport_rx(buffer, buf_len, message_len, src, timeout_ms);
    if (!success) {
        uart_transmit_formatted_message("timed out.\r\n", *src);
        UART_WAIT_UNTIL_DONE();
        return false;
    }

    // force the string to be null-terminated if it isn't already
    buffer[buf_len - 1] = '\0';

    // report that the message was received
    uart_transmit_formatted_message("\r\n========== Received message from %02x ==========\r\n", *src);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message(buffer);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message("================================================\r\n");
    UART_WAIT_UNTIL_DONE();

    // record it
    log_message(buffer, *message_len, *src);        

    return true;
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

        // receive a message
        if (listen(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this, TRX_TIMEOUT_INDEFINITE)) {

            num_messages_this_session++;

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
}

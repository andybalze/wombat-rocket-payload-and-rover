#include "application.h"
#include "transport.h"
#include "networking_constants.h"
#include "uart.h"
#include "address.h"
#include "digital_io.h"
#include "log.h"

#include <stdio.h>
#include <string.h>

#include "cube_parameters.h"
#include <util/delay.h>

#define ROVER_APP_DELAY_BETWEEN_MSG_MS (2000UL)

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

    uint16_t num_messages_this_session = 0;

    uart_transmit_formatted_message("::: Rover's transceiver activated. Entering network mode. :::\r\n");
    UART_WAIT_UNTIL_DONE();

    LED_set(LED_WHITE);


    // This block of code is what sets the rover apart. It is the sole transmitter in the network.
    _delay_ms(ROVER_APP_DELAY_BETWEEN_MSG_MS);
    snprintf(message, MAX_MESSAGE_LEN, "Hello, data cube at address 0a. I am the Wombat. Please turn red, like a tomato.\r\nDo you pronounce it tomato, or tomato?\r\nLED:RED\r\n");
    transport_tx(message, strlen(message)+1, 0x0A);

    _delay_ms(ROVER_APP_DELAY_BETWEEN_MSG_MS);
    snprintf(message, MAX_MESSAGE_LEN, "Hello, data cube at address 0b. I am the Wombat. Please turn cyan.\r\nTeal and cyan are nice colors. They remind me of the ocean.\r\nLED:CYAN\r\n");
    transport_tx(message, strlen(message)+1, 0x0B);

    _delay_ms(ROVER_APP_DELAY_BETWEEN_MSG_MS);
    snprintf(message, MAX_MESSAGE_LEN, "Hello, data cube at address 0c. I am the Wombat. Please turn magenta.\r\nThe best way to enjoy plum pie is with vanilla ice cream to the side.\r\nLED:MAGENTA\r\n");
    transport_tx(message, strlen(message)+1, 0x0C);

    // After this point, the rover actually behaves pretty similarly to the data cubes.
    while(true) {

        // listen for a message
        for (int i = 0; i < MAX_MESSAGE_LEN; i++) message[i] = 0;
        transport_rx(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this);

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
        snprintf(message, MAX_MESSAGE_LEN, "Hello, whoever lives at address %02x. I am the Wombat, at address %02x.\r\nI have received %d messages since powering on.\r\nThanks for reaching out.\r\n", who_sent_me_this, MY_PORT, num_messages_this_session);

        // transmit the response
        // note: the +1 is to include the null terminator in the message
        transport_tx(message, strlen(message) + 1, who_sent_me_this);

        // DEBUG! Remove this after you've determined the EEPROM isn't being written to a whole bunch.
        _delay_ms(1000);

    }
}

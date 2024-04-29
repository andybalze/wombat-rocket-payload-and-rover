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

// Same as transport_rx, with the side effects of
// logging the result and printing the message to UART.
void listen(byte* buffer, uint16_t buf_len, uint16_t* message_len, byte* src) {

    for (int i = 0; i < buf_len; i++) buffer[i] = 0;
    transport_rx(buffer, buf_len, message_len, src);

    // force the string to be null-terminated if it isn't already
    buffer[buf_len - 1] = '\0';

    // report that the message was received
    uart_transmit_formatted_message("========== Received message from %02x ==========\r\n", *src);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message(buffer);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message("================================================\r\n");
    UART_WAIT_UNTIL_DONE();

    // record it
    log_message(buffer, *message_len, *src);        

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


    // ================= TRANSMIT TO CUBE 0 ===========================
    _delay_ms(ROVER_APP_DELAY_BETWEEN_MSG_MS);
    snprintf(message, MAX_MESSAGE_LEN, "Hello, data cube at address 0a. I am the Wombat. Please turn red, like a tomato.\r\nDo you pronounce it tomato, or tomato?\r\nLED:RED\r\n");
    transport_tx(message, strlen(message)+1, 0x0A);

    // LISTEN FOR CUBE 0's RESPONSE
    listen(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this);

    // ================= TRANSMIT TO CUBE 1 ===========================
    _delay_ms(ROVER_APP_DELAY_BETWEEN_MSG_MS);
    snprintf(message, MAX_MESSAGE_LEN, "Hello, data cube at address 0b. I am the Wombat. Please turn cyan.\r\nTeal and cyan are nice colors. They remind me of the ocean.\r\nLED:CYAN\r\n");
    transport_tx(message, strlen(message)+1, 0x0B);

    // LISTEN FOR CUBE 1's RESPONSE
    listen(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this);

    // ================= TRANSMIT TO CUBE 2 ===========================
    _delay_ms(ROVER_APP_DELAY_BETWEEN_MSG_MS);
    snprintf(message, MAX_MESSAGE_LEN, "Hello, data cube at address 0c. I am the Wombat. Please turn magenta.\r\nThe best way to enjoy plum pie is with vanilla ice cream to the side.\r\nLED:MAGENTA\r\n");
    transport_tx(message, strlen(message)+1, 0x0C);

    // LISTEN FOR CUBE 2's RESPONSE
    listen(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this);

    // After this point, the rover actually behaves pretty similarly to the data cubes.
    while(true) {

        // receive a message
        listen(message, MAX_MESSAGE_LEN, &message_len, &who_sent_me_this);

        num_messages_this_session++;

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

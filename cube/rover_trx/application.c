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


// call transport_tx and handle the error messages.
void application_tx(byte* message, uint16_t message_len, byte dest_port) {
    transport_tx_result result;
    result = transport_tx(message, 79, dest_port);
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

    char *color_wheel_str[7] = {
        "Go touch some grass. LED:GREEN\r\n",
        "This color reminds me of the ocean. LED:CYAN\r\n",
        "Is it pronounced \"tomato\" or \"tomato\"? LED:RED\r\n",
        "This color is pretty. LED:MAGENTA\r\n",
        "This is yellow? Are you sure? LED:YELLOW\r\n",
        "White chocolate is over-rated. Except when used in cookies. LED:WHITE\r\n",
        "Do you like blue? LED:BLUE\r\n",
    };

    char color_wheel_raw[7] = {
        LED_GREEN,
        LED_CYAN,
        LED_RED,
        LED_MAGENTA,
        LED_YELLOW,
        LED_WHITE,
        LED_BLUE,
    };

    uint8_t everyone[3] = {
        0x3a,
        0x3b,
        0x3c
    };

    uart_transmit_formatted_message("::: Rover's transceiver activated. Entering network mode. :::\r\n");
    UART_WAIT_UNTIL_DONE();

    LED_set(LED_BLUE);

    _delay_ms(1000);

    while(true) {

        // Step right up and spin the wheel!
        for (int i = 0; i < 7; i++) {
            char* this_color_str = color_wheel_str[i];
            char this_color_raw = color_wheel_raw[i];

            // Show off the color we chose.
            LED_set(this_color_raw);
            for (int i = 0; i < 5; i++) {
                LED_blink(this_color_raw);
            }
            _delay_ms(1000);

            // Alright, now everybody has to wear it.
            for (int j = 0; j < 3; j++) {
                snprintf(message, MAX_MESSAGE_LEN, this_color_str);
                application_tx(message, strlen(message), everyone[j]);
                _delay_ms(5000);
            }
        }
    }
}

#include "application.h"
#include "transport.h"
#include "networking_constants.h"
#include "uart.h"

// for testing
#include "data_link.h"

void application() {

    byte received_message[MAX_MESSAGE_LEN];


    // For testing. Are we actually using just 30% of memory??
    byte test_buffer[MAX_FRAME_LEN] = "test";
    for (int i = 0; i < 255; i++) {
        data_link_push_to_fifo(test_buffer, MAX_FRAME_LEN);
    }

    // To be written.
    while(1) {

        transport_rx(received_message, MAX_MESSAGE_LEN);
        uart_transmit_formatted_message(received_message);

    }
}

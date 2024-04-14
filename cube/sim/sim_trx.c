/*

        WARNING

        This is a SIMULATION FILE.
        Do not compile this code for hardware.

*/

#include "sim_trx.h"

// This file simulates the behavior of the transceiver.
// It requires Linux-style FIFOs because Windows makes
// things hard. This code will only compile and run on Linux.
// WSL might work?

// If you get red squigglies here, you're probably on Windows

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/////////////////// Private type definitions ///////////////////////////////////

static trx_address_t my_addr;
char my_fifo[256];



/////////////////// Public Function Bodies /////////////////////////////////////

// Initializes the TRX, including initializing the SPI and any other peripherals
// required.
void trx_initialize(trx_address_t rx_address) {
    my_addr = rx_address;
    sprintf(my_fifo, "/tmp/rocket_rover_fifo_test_%#010x", rx_address);
    printf("Creating a FIFO: %s\n", my_fifo);
    mkfifo(my_fifo, 0666);
}

// Transmits a payload to the given address.
trx_transmission_outcome_t trx_transmit_payload(
    trx_address_t address,
    trx_payload_element_t *payload,
    int payload_length
) {

    // prepare FIFO name
    char destination_fifo[256];
    sprintf(destination_fifo, "/tmp/rocket_rover_fifo_test_%#010x", address);

    // prepare payload
    char final_payload[TRX_PAYLOAD_LENGTH];
    for (int i = 0; i < TRX_PAYLOAD_LENGTH; i++) {
        final_payload[i] = '\0';
    }
    if (!(payload_length < TRX_PAYLOAD_LENGTH - 1)) {
        payload_length = TRX_PAYLOAD_LENGTH - 1;
    }
    for (int i = 0; i < payload_length; i++) {
        final_payload[i] = payload[i];
    }



    // Open FIFO
    int fd = open(destination_fifo, O_WRONLY);
    printf("Test..\n");

    // Write to FIFO
    write(fd, final_payload, TRX_PAYLOAD_LENGTH);

    // Close FIFO
    close(fd);
    return TRX_TRANSMISSION_SUCCESS;


}

// Receives a payload using polling.
int trx_receive_payload(trx_payload_element_t *payload_buffer) {

    // Open FIFO
    int fd = open(my_fifo, O_RDONLY);

    if (fd) {
        // Read from FIFO
        read(fd, payload_buffer, TRX_PAYLOAD_LENGTH);

        // Close the FIFO
        close(fd);
    }
    else {
        printf("Error opening FIFO to read: %s\n", my_fifo);
    }
    return TRX_PAYLOAD_LENGTH;

}

// You silly goose
trx_status_buffer_t trx_get_status() {
    return 0;
}

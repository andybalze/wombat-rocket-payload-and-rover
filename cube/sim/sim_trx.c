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
#include <poll.h>

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
    if (!(payload_length < TRX_PAYLOAD_LENGTH)) {
        payload_length = TRX_PAYLOAD_LENGTH;
    }
    for (int i = 0; i < payload_length; i++) {
        final_payload[i] = payload[i];
    }



    // Open FIFO
    int fd = open(destination_fifo, O_WRONLY);

    // Write to FIFO
    write(fd, final_payload, TRX_PAYLOAD_LENGTH);

    // Close FIFO
    close(fd);
    return TRX_TRANSMISSION_SUCCESS;


}

// Receives a payload using polling.
trx_reception_outcome_t trx_receive_payload(
  trx_payload_element_t *payload_buffer,
  uint16_t timer_delay_ms
) {
    // https://man7.org/linux/man-pages/man2/poll.2.html
    int ready;
    ssize_t s;
    struct pollfd fds;


    // Open FIFO
    fds.fd = open(my_fifo, O_RDONLY | O_NONBLOCK);
    if (fds.fd == -1) {
        printf("ERROR: Could not open FIFO.\n");
        return TRX_RECEPTION_FAILURE;
    }

    int my_delay = timer_delay_ms;
    if (my_delay >= TRX_TIMEOUT_INDEFINITE)
        my_delay = -1;

    // Wait for data
    ready = poll(&fds, 1, my_delay);
    if (ready == -1) {
        return TRX_RECEPTION_FAILURE;
    }
    if (fds.revents == 0) {
        return TRX_RECEPTION_FAILURE;
    }


    // Read from FIFO
    s = read(fds.fd, payload_buffer, TRX_PAYLOAD_LENGTH);
    if (s == -1) {
        printf("reading failed, s == -1\n");
        return TRX_RECEPTION_FAILURE;
    }

    // Close the FIFO
    if (close(fds.fd) == -1) {
        printf("closing the file failed\n");
        return TRX_RECEPTION_FAILURE;
    }

    return TRX_RECEPTION_SUCCESS;

}

// You silly goose
trx_status_buffer_t trx_get_status() {
    return 0;
}

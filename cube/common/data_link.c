#include "networking_constants.h"
#include "data_link.h"

int data_link_rx(char* buffer, int buf_len) {

    // Wait until the FIFO has data.

    // Pull from the fifo, copy the data to buffer,
    // and return.

}

void data_link_tx(char* buffer, int buf_len) {

    // Implementation TBD

}

#include "networking_constants.h"
#include "data_link.h"

#define FIFO_DEPTH (4)

// frame[0] = frame length
// rest is payload

// --------------------------- FIFO Code ---------------------------------------
// This FIFO works assuming one ISR writes to it, and the main loop
// reads from it.
// If this assumption is not true, this FIFO will require mutex locks.

// Tread carefully. This is concurrent programming, which has quite a few
// pitfalls.

static volatile char fifo[FIFO_DEPTH][MAX_FRAME_LEN]; // Double-check I got this right
static volatile int fifo_start = 0;
static volatile int fifo_end = 0;
static volatile int fifo_words_used = 0;

// Add data to the FIFO.
// Returns a 1 if successful or a 0 if not.
// I expect Michael's ISR to call this function.
int data_link_push_to_fifo(char* buffer, int buf_len) {

    if (fifo_words_used == FIFO_DEPTH) {
        return 0;
    }
    else {
        // Add data to FIFO
        for (int i = 0; i < buf_len && i < MAX_FRAME_LEN; i++) {
            fifo[fifo_end][i] = buffer[i]; // double-check I got this right
        }
        // Increment pointer, wrap around if necessary
        fifo_end += 1;
        if (fifo_end == FIFO_DEPTH) {
            fifo_end = 0;
        }
        fifo_words_used += 1;
        return 1;
    }
}


// Wait until we have data ready in our FIFO buffer, and
// store it to the buffer. Return 1 if successful.
int data_link_pull_from_fifo(char* buffer, int buf_len) {

    // Wait until the FIFO has data.
    while (fifo_words_used == 0);

    // Pull from the FIFO. The order is pretty important here to avoid
    // an interrupt corrupting stuff.
    for (int i = 0; i < buf_len && i < MAX_FRAME_LEN; i++) {
        buffer[i] = fifo[fifo_start][i];
    }
    fifo_start += 1;
    if (fifo_start == FIFO_DEPTH) {
        fifo_start = 0;
    }
    fifo_words_used -= 1;

    return 1;

}


// ---------------------------- NETWORKING INTERFACE ---------------------------

// This blocking function gets a payload from the data link layer
// and writes it to the buffer.
// It returns the length of the payload (i.e. the packet).
int data_link_rx(char* buffer, int buf_len) {


    // Pull data from the fifo.
    char frame[MAX_FRAME_LEN];
    data_link_pull_from_fifo(frame, MAX_FRAME_LEN);
    
    // Identify the length of this frame.
    int frame_len = frame[0];

    // Now copy just the payload into the buffer.
    for (int i = 0; i < frame_len - FRAME_HEADER_LEN && i < buf_len; i++) {
        buffer[i] = frame[i + FRAME_HEADER_LEN];
    }

    return frame_len - FRAME_HEADER_LEN;
}

void data_link_tx(char* payload, int payload_len, int addr) {

    // Implementation TBD
    // Whatever this does, it'll most likely call Michael's trx_transmit function.

}

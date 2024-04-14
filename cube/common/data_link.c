#include "data_link.h"
#include "address.h"


#ifndef SIMULATION
#include "trx.h"
#else
#include "sim_trx.h"
#endif

#define FIFO_DEPTH (4)

// frame[0] = frame length
// rest is payload

// --------------------------- FIFO Code ---------------------------------------
// This FIFO works assuming one ISR writes to it, and the main loop
// reads from it.
// If this assumption is not true, this FIFO will require mutex locks.

// Tread carefully. This is concurrent programming, which has quite a few
// pitfalls.

static volatile byte fifo[FIFO_DEPTH][MAX_FRAME_LEN]; // Double-check I got this right
static volatile byte fifo_start = 0;
static volatile byte fifo_end = 0;
static volatile byte fifo_words_used = 0;

// Add data to the FIFO.
// Returns true if successful.
// I expect Michael's ISR to call this function.
bool data_link_push_to_fifo(byte* buffer, byte buf_len) {

    if (fifo_words_used == FIFO_DEPTH) {
        return false;
    }
    else {
        // Add data to FIFO
        for (byte i = 0; i < buf_len && i < MAX_FRAME_LEN; i++) {
            fifo[fifo_end][i] = buffer[i]; // double-check I got this right
        }
        // Increment pointer, wrap around if necessary
        fifo_end += 1;
        if (fifo_end == FIFO_DEPTH) {
            fifo_end = 0;
        }
        fifo_words_used += 1;
        return true;
    }
}


// Wait until we have data ready in our FIFO buffer, and
// store it to the buffer. Return true if successful.
bool data_link_pull_from_fifo(byte* buffer, byte buf_len) {

    // Wait until the FIFO has data.
    while (fifo_words_used == 0);

    // Pull from the FIFO. The order is pretty important here to avoid
    // an interrupt corrupting stuff.
    for (byte i = 0; i < buf_len && i < MAX_FRAME_LEN; i++) {
        buffer[i] = fifo[fifo_start][i];
    }
    fifo_start += 1;
    if (fifo_start == FIFO_DEPTH) {
        fifo_start = 0;
    }
    fifo_words_used -= 1;

    return true;

}


// ---------------------------- NETWORKING INTERFACE ---------------------------

// This blocking function gets a payload from the data link layer
// and writes it to the buffer.
// It returns the length of the payload (i.e. the packet).
byte data_link_rx(byte* buffer, byte buf_len) {


    // Pull data from the fifo.
    byte frame[MAX_FRAME_LEN];
    data_link_pull_from_fifo(frame, MAX_FRAME_LEN);
    
    // Identify the length of this frame.
    byte frame_len = frame[0];

    // Now copy just the payload into the buffer.
    for (byte i = 0; i < frame_len - FRAME_HEADER_LEN && i < buf_len; i++) {
        buffer[i] = frame[i + FRAME_HEADER_LEN];
    }

    return frame_len - FRAME_HEADER_LEN;
}

void data_link_tx(byte* payload, byte payload_len, uint32_t addr) {

    // Implementation TBD
    // Whatever this does, it'll most likely call Michael's trx_transmit function.

}

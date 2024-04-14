/*
    Welcome to what is perhaps the most barebones transport layer
    mankind has ever created. A modern transport layer like TCP is pretty nuts.

    The segment includes some unused data. This is in case we want to
    be a little more elaborate.
*/

#include "address_resolution.h"
#include "transport.h"
#include "network.h"
#include "address.h"

// the transport layer intentionally adds delays between segments.
// fun fact: in the TCP/IP protocol stack,
// the transport layer is actually responsible for preventing
// a network from being overwhelmed by throttling its own output!

#include "cube_parameters.h"

#ifndef SIMULATION
#include <util/delay.h>
#else
#include "sim_delay.h"
#endif

#define TRANSPORT_DELAY_MS (100)


// three segment types: START_OF_MESSAGE, DATA, END_OF_MESSAGE

// START_OF_MESSAGE segment:
// segment[0] = length of segment = 5
// segment[1] = destination port number
// segment[2] = source port number
// segment[3] = segment identifier = 0x0A, START_OF_MESSAGE
// segment[4] = total length of message

// DATA segment:
// segment[0] = length of segment
// segment[1] = destination port number
// segment[2] = source port number
// segment[3] = segment identifier = 0x0D, DATA
// segment[4] = start address (starting memory address of this segment's data)
// rest is payload

// END_OF_MESSAGE segment:
// segment[0] = length of segment = 4
// segment[1] = destination port number
// segment[2] = source port number
// segment[3] = segment identifier = 0x0B, END_OF_MESSAGE

// Using an enum for a "state machine" to make this a little more easily expandable.
enum rx_state_t {
    RXST_Idle,
    RXST_Receiving,
    RXST_Complete
};

enum segment_identifier_t {
    SEGID_START_OF_MESSAGE = 0x0A,
    SEGID_DATA = 0x0D,
    SEGID_END_OF_MESSAGE = 0x0B
};




byte transport_rx(byte* buffer, byte buf_len) {

    int state = RXST_Idle;
    byte message_length;

    byte segment_len;
    byte segment[MAX_SEGMENT_LEN];

    byte segment_identifier;

    do {

        // Get the next segment
        network_rx(segment, MAX_SEGMENT_LEN);
        segment_len = segment[0];
        segment_identifier = segment[3];

        // Then do something with it
        switch(state) {

        case RXST_Idle:
            if (segment_identifier == SEGID_START_OF_MESSAGE) {
                message_length = segment[4];
                state = RXST_Receiving;
            }
            break;

        case RXST_Receiving:
            if (segment_identifier == SEGID_DATA) {
                byte offset = segment[4];
                byte payload_len = segment_len - DATA_SEGMENT_HEADER_LEN;
                for (byte i = 0; i < payload_len; i++) {
                    if (i + offset < buf_len) {
                        buffer[i + offset] = segment[i + DATA_SEGMENT_HEADER_LEN];
                    }
                }
            }
            else if (segment_identifier == SEGID_END_OF_MESSAGE) {
                state = RXST_Complete;
            }
            break;

        // (this case never executes)
        case RXST_Complete:
            break;

        }

    } while (state != RXST_Complete);


    // We have a complete message!
    state = RXST_Idle;
    return message_length;
}

void transport_tx(byte* message, byte message_len, byte dest_port) {

    byte segment[MAX_SEGMENT_LEN];
    byte bytes_remaining = message_len;

    // ------ send START_OF_MESSAGE -----
    segment[0] = START_SEGMENT_HEADER_LEN;
    segment[1] = dest_port;
    segment[2] = MY_PORT;
    segment[3] = SEGID_START_OF_MESSAGE;
    segment[4] = message_len;

    network_tx(segment, START_SEGMENT_HEADER_LEN, resolve_network_addr(dest_port), MY_NETWORK_ADDR);
    _delay_ms(TRANSPORT_DELAY_MS);


    // ------ send data segments -----
    while (bytes_remaining > 0) {

        // can't send the whole darn thing at once unless it's small
        byte this_payload_len;
        if (bytes_remaining > MAX_SEGMENT_LEN - DATA_SEGMENT_HEADER_LEN) {
            this_payload_len = MAX_SEGMENT_LEN - DATA_SEGMENT_HEADER_LEN;
        }
        else {
            this_payload_len = bytes_remaining;
        }

        byte this_segment_len = this_payload_len + DATA_SEGMENT_HEADER_LEN;
        byte start_address = message_len - bytes_remaining;

        segment[0] = this_segment_len;
        segment[1] = dest_port;
        segment[2] = MY_PORT;
        segment[3] = SEGID_DATA;
        segment[4] = start_address;
        for (byte i = 0; i < this_payload_len; i++) {
            segment[i + DATA_SEGMENT_HEADER_LEN] = message[i + start_address];
        }

        bytes_remaining -= this_payload_len;
        network_tx(segment, this_segment_len, resolve_network_addr(dest_port), MY_NETWORK_ADDR);
        _delay_ms(TRANSPORT_DELAY_MS);
    }

    // ------ send END_OF_MESSAGE -----
    segment[0] = END_SEGMENT_HEADER_LEN;
    segment[1] = dest_port;
    segment[2] = MY_PORT;
    segment[3] = SEGID_END_OF_MESSAGE;

    network_tx(segment, END_SEGMENT_HEADER_LEN, resolve_network_addr(dest_port), MY_NETWORK_ADDR);

    return;

}

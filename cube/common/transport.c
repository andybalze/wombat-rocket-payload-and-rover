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

#define TRANSPORT_ACK_DELAY_MS (1)
#define TRANSPORT_TIMEOUT_MS (3)


// three segment types: START_OF_MESSAGE, DATA, END_OF_MESSAGE

// START_OF_MESSAGE segment:
// segment[0] = length of segment = 6
// segment[1] = sequence number
// segment[2] = destination port number
// segment[3] = source port number
// segment[4] = segment identifier = 0x07, START_OF_MESSAGE
// segment[5] = total length of message

// DATA segment:
// segment[0] = length of segment
// segment[1] = sequence number
// segment[2] = destination port number
// segment[3] = source port number
// segment[4] = segment identifier = 0x0D, DATA
// segment[5] = start address (starting memory address of this segment's data)
// rest is payload

// END_OF_MESSAGE segment:
// segment[0] = length of segment = 5
// segment[1] = sequence number
// segment[2] = destination port number
// segment[3] = source port number
// segment[4] = segment identifier = 0x09, END_OF_MESSAGE

// ACK segment:
// segment[0] = length of segment = 5
// segment[1] = sequence number
// segment[2] = destination port number
// segment[3] = source port number
// segment[4] = segment identifier = 0x0A, ACK


// Using an enum for a "state machine" to make this a little more easily expandable.
enum rx_state_t {
    RXST_Idle,
    RXST_Receiving,
    RXST_Complete
};

enum segment_identifier_t {
    SEGID_START_OF_MESSAGE = 0x07,
    SEGID_DATA = 0x0D,
    SEGID_END_OF_MESSAGE = 0x09,
    SEGID_ACK = 0x0A
};



// While receiving...
// if I receive a frame, I first acknowledge it no matter what.
// If the frame's sequence number is 0, I ack 1 and set my "expecting seq num" to 1.
// If the frame's sequence number is 1, I ack 0 and set my "expecting seq num" to 0.

// If the frame's sequence number matches my "expecting seq num", we're golden.
// I'll do something with this frame.
// If it does not match... well, somehow my ack got lost in transmission,
// so the other guy sent the same thing again. I'll ignore it.

byte transport_rx(byte* buffer, byte buf_len) {

    int state = RXST_Idle;
    byte message_length;

    byte segment_len;
    byte segment[MAX_SEGMENT_LEN];

    byte ack_seg[ACK_SEGMENT_HEARDER_LEN];

    byte segment_identifier;

    byte expecting_seq_number = 0;

    do {

        // Get the next segment
        bool success = network_rx(segment, MAX_SEGMENT_LEN, 255);
        if (success) {

            segment_len = segment[0];
            segment_identifier = segment[4];

            // No matter my state, I will acknowledge it...
            ack_seg[0] = ACK_SEGMENT_HEARDER_LEN;
            ack_seg[1] = segment[1] == 0 ? 1 : 0; // advance seq number (don't kill me)
            ack_seg[2] = segment[3]; // destination port = port of whoever sent
            ack_seg[3] = MY_PORT;    // source port = me :)
            ack_seg[4] = SEGID_ACK;
            network_tx(ack_seg, ACK_SEGMENT_HEARDER_LEN, resolve_network_addr(segment[3]), MY_NETWORK_ADDR);

            // Okay. Do I do something with it now?
            // Only if it was the sequence number I was expecting.
            if (expecting_seq_number == segment[1]) {

                // Okay, this is fresh data I haven't seen before.
                // Let's work with it.
                switch(state) {

                case RXST_Idle:
                    if (segment_identifier == SEGID_START_OF_MESSAGE) {
                        message_length = segment[5];
                        state = RXST_Receiving;
                    }
                    break;

                case RXST_Receiving:
                    if (segment_identifier == SEGID_DATA) {
                        byte offset = segment[5];
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
            }
            // Finally. I can update my expecting_seq_number.
            expecting_seq_number = segment[1] == 0 ? 1 : 0;
        }

    } while (state != RXST_Complete);


    // We have a complete message!
    state = RXST_Idle;
    return message_length;
}



// While transmitting...
// We both start at seq number 0. This will be my "current seq num".

// I will send a message. Then I'll immediately start waiting for an ack.
// If I timeout, I will re-send my message.
// If I get an ack, but the ack's sequence number matches mine,
// I will re-send my message.
// If I get an ack, and the ack sequence number is advanced one,
// I can finally adjust "current seq number" and move on and send the next packet.


// This function is local to this file.
// It takes in a ready-to-go segment and tries to transport it until
// it gets an acknowledgement.
bool transport_keep_trying_to_tx(byte* segment, byte segment_len, byte dest_port, byte* current_seq_num) {

    byte hopefully_an_ack[ACK_SEGMENT_HEARDER_LEN];

    bool did_my_friend_get_the_message_yet = false;

    do {
        // Let's send this bad boy.
        network_tx(segment, segment_len, resolve_network_addr(dest_port), MY_NETWORK_ADDR);
        // And see if it came.
        if (network_rx(hopefully_an_ack, ACK_SEGMENT_HEARDER_LEN, TRANSPORT_TIMEOUT_MS)) {
            // No timeout, good...
            if (hopefully_an_ack[4] == SEGID_ACK) {
                // Ack, good...
                if (hopefully_an_ack[1] != *current_seq_num) {
                    // They got it! (Sequence numbers are diff. so they expect something new)
                    did_my_friend_get_the_message_yet = true;
                }
            }
        }

    } while (did_my_friend_get_the_message_yet == false);

    *current_seq_num = *current_seq_num == 0 ? 1 : 0;

    // for now, this will NOT return until my MESSAGE WAS ACKNOWLEDGED, DARNIT!
    return true;
}

void transport_tx(byte* message, byte message_len, byte dest_port) {

    byte current_seq_num = 0;

    byte segment[MAX_SEGMENT_LEN];
    byte bytes_remaining = message_len;

    // ------ send START_OF_MESSAGE -----
    segment[0] = START_SEGMENT_HEADER_LEN;
    segment[1] = current_seq_num;
    segment[2] = dest_port;
    segment[3] = MY_PORT;
    segment[4] = SEGID_START_OF_MESSAGE;
    segment[5] = message_len;

    transport_keep_trying_to_tx(segment, START_SEGMENT_HEADER_LEN, dest_port, &current_seq_num);

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
        segment[1] = current_seq_num;
        segment[2] = dest_port;
        segment[3] = MY_PORT;
        segment[4] = SEGID_DATA;
        segment[5] = start_address;
        for (byte i = 0; i < this_payload_len; i++) {
            segment[i + DATA_SEGMENT_HEADER_LEN] = message[i + start_address];
        }

        bytes_remaining -= this_payload_len;

        transport_keep_trying_to_tx(segment, this_segment_len, dest_port, &current_seq_num);
    }

    // ------ send END_OF_MESSAGE -----
    segment[0] = END_SEGMENT_HEADER_LEN;
    segment[1] = current_seq_num;
    segment[2] = dest_port;
    segment[3] = MY_PORT;
    segment[4] = SEGID_END_OF_MESSAGE;

    transport_keep_trying_to_tx(segment, END_SEGMENT_HEADER_LEN, dest_port, &current_seq_num);

    return;

}

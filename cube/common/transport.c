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
#include "cube_parameters.h"

#ifndef SIMULATION
#include "trx.h"
#include <util/delay.h>
#else
#include "sim_trx.h"
#include "sim_delay.h"
#include "print_data.h"
#include <stdio.h>
#endif

// the transport layer intentionally adds delays between segments.
// fun fact: in the TCP/IP protocol stack,
// the transport layer is actually responsible for preventing
// a network from being overwhelmed by throttling its own output!


#define TRANSPORT_ACK_DELAY_MS (2000)
#define TRANSPORT_TIMEOUT_MS (4000)
#define TRANSPORT_TX_ATTEMPT_LIMIT (5)


// four segment types: START_OF_MESSAGE, DATA, END_OF_MESSAGE, ACK

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
    RXST_Receiving
};



/*
================================================================================

            Theory

=== Receiver side ===
if I receive a frame, I first acknowledge it no matter what.
If the frame's sequence number is 0, I ack 1, regardless of what my expected_seq_num is.
If the frame's sequence number is 1, I ack 0, regardless of what my expected_seq_num is.

If the frame's sequence number matches my "expected_seq_num", we're golden.
I'll do something with this frame. I will also advance my expected_seq_num.
If it does not match... well, somehow my previous ack got lost in transmission,
so the other guy sent the same thing again. I won't do anything this time.
Hopefully he got my ack this time and sends the next frame.

=== Transmitter side ===
We both start at seq number 0. This will be my "current seq num".

I will send a message. Then I'll immediately start waiting for an ack.
If I timeout, I will re-send my message.
If I get an ack, but the ack's sequence number matches mine,
I will re-send my message.
If I get an ack, and the ack sequence number is advanced one,
I can finally adjust "current seq number" and move on and send the next packet.

================================================================================
*/




// ======================= Receiver Code =======================================



// Get data from the network layer.
// Acknowledge it.
// Verify the data is new by checking the sequence number.
// Returns true if the reception was successful.
bool transport_attempt_rx(byte* segment, byte buf_len, byte expected_seq_num) {

    bool success;
    byte ack_seg[ACK_SEGMENT_HEARDER_LEN];

    // try to receive some data from the network
    success = network_rx(segment, MAX_SEGMENT_LEN, TRX_TIMEOUT_INDEFINITE);

    // Timed out?
    if (!success) {
        return false;
    }

    // Alright we got something, let me acknowledge it really quick.
    _delay_ms(TRANSPORT_ACK_DELAY_MS);
    ack_seg[0] = ACK_SEGMENT_HEARDER_LEN;
    ack_seg[1] = segment[1] == 0 ? 1 : 0; // advance seq number
    ack_seg[2] = segment[3]; // destination port = port of whoever sent
    ack_seg[3] = MY_PORT;    // source port = me :)
    ack_seg[4] = SEGID_ACK;
    // (the return value of this function call is intentionally ignored)
    network_tx(ack_seg, ACK_SEGMENT_HEARDER_LEN, resolve_network_addr(segment[3]), MY_NETWORK_ADDR);


    // Okay. Is this new data?
    if (expected_seq_num != segment[1]) {
        return false;
    }

    return true;
}

// For now, this will never fail.
// You could make it fail after a number of attempts... would there be a point?
bool transport_keep_trying_to_rx(byte* segment, byte buf_len, byte expected_seq_num) {
    while(true) {
        if (transport_attempt_rx(segment, buf_len, expected_seq_num)) {
            return true;
        }
    }
}


// The application layer calls this function.
// Get a complete message.
// The function returns if a complete message was successfully received.
bool transport_rx(byte* buffer, byte buf_len) {

    int state = RXST_Idle;
    byte message_length;

    byte segment_len;
    byte segment[MAX_SEGMENT_LEN];

    byte segment_identifier;
    byte expected_seq_num = 0;

    // Continually receive segments until we've put together a whole message.
    while(true) {

        // Get the next segment.
        // As a side effect, acknowledge anything we receive.
        bool success = transport_keep_trying_to_rx(segment, MAX_SEGMENT_LEN, expected_seq_num);
        if (!success)
            return false;

        expected_seq_num = expected_seq_num == 0 ? 1 : 0;

        segment_len = segment[0];
        segment_identifier = segment[4];

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

                for (byte i = 0; i < payload_len && i + offset < buf_len; i++) {
                    buffer[i + offset] = segment[i + DATA_SEGMENT_HEADER_LEN];
                }
            }
            else if (segment_identifier == SEGID_END_OF_MESSAGE) {
                return true;
            }
            // An unusual case. Can happen if the transmitter gives up
            // and tries again from the beginning.
            else if (segment_identifier == SEGID_START_OF_MESSAGE) {
                message_length = segment[5];
            }
            break;
        }
    }
}



// ======================= Transmitter Code ====================================


// This function transmits a segment, then waits to receive an acknowledgement.
// This function can time out.
// The function returns whether the acknowledgement was received before the timeout.
bool transport_attempt_tx(byte* segment, byte segment_len, byte dest_port, byte current_seq_num) {

    byte hopefully_an_ack[ACK_SEGMENT_HEARDER_LEN];
    bool success;

    // Let's send this bad boy.
    success = network_tx(segment, segment_len, resolve_network_addr(dest_port), MY_NETWORK_ADDR);

    // Did the transmission go through?
    if (!success) {
        return false;
    }

    // Now let's try to get an acknowledgement.
    success = network_rx(hopefully_an_ack, ACK_SEGMENT_HEARDER_LEN, TRANSPORT_TIMEOUT_MS);

    // Timed out?
    if (!success) {
        return false;
    }

    // The received segment is an ack?
    if (hopefully_an_ack[4] != SEGID_ACK) {
        return false;
    }

    // The sequence number is advanced?
    if (hopefully_an_ack[1] == current_seq_num) {
        return false;
    }

    // Looks good. Our message was acknowledged.
    return true;
}

// This function continually tries to transmit segments until one is acknowledged.
// This function can fail if the attempted transmissions exceeds TRANSPORT_TX_ATTEMPT_LIMIT.
// The function returns whether the segment was eventually transmitted and acknowledged.
bool transport_keep_trying_to_tx(byte* segment, byte segment_len, byte dest_port, byte current_seq_num) {

    uint16_t transmit_attempts = 0;
    bool success;

    while(true) {

        transmit_attempts++;
        if (transmit_attempts > TRANSPORT_TX_ATTEMPT_LIMIT)
            return false;

        success = transport_attempt_tx(segment, segment_len, dest_port, current_seq_num);

        if (success)
            return true;
    }
}

// The application layer calls this function.
// The function takes the message, splits it up into segments,
// and sends each segment one-by-one.
// Every segment must be acknowledged before the next one is sent.
// The function can fail if one of the segments is not acknowledged in time.
// The function returns whether the message was sent successfully.
bool transport_tx(byte* message, byte message_len, byte dest_port) {

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

    if (!transport_keep_trying_to_tx(segment, START_SEGMENT_HEADER_LEN, dest_port, current_seq_num))
        return false;
    current_seq_num = current_seq_num == 0 ? 1 : 0;

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

        if (!transport_keep_trying_to_tx(segment, this_segment_len, dest_port, current_seq_num))
            return false;
        current_seq_num = current_seq_num == 0 ? 1 : 0;
    }

    // ------ send END_OF_MESSAGE -----
    segment[0] = END_SEGMENT_HEADER_LEN;
    segment[1] = current_seq_num;
    segment[2] = dest_port;
    segment[3] = MY_PORT;
    segment[4] = SEGID_END_OF_MESSAGE;

    if (!transport_keep_trying_to_tx(segment, END_SEGMENT_HEADER_LEN, dest_port, current_seq_num))
        return false;
    current_seq_num = current_seq_num == 0 ? 1 : 0;

    return true;

}

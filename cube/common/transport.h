#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

enum segment_identifier_t {
    SEGID_START_OF_MESSAGE = 0x07,
    SEGID_DATA = 0x0D,
    SEGID_END_OF_MESSAGE = 0x09,
    SEGID_ACK = 0x0A
};

bool transport_rx(byte* buffer, uint16_t buf_len, uint16_t* message_len, byte* source_port);

bool transport_tx(byte* message, uint16_t message_len, byte dest_port);

#endif

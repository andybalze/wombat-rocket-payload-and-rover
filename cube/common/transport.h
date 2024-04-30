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

typedef enum {
    TRANSPORT_RX_SUCCESS,
    TRANSPORT_RX_TIMEOUT,
    TRANSPORT_RX_ERROR
} transport_rx_result;

typedef enum {
    TRANSPORT_TX_SUCCESS,
    TRANSPORT_TX_REACHED_ATTEMPT_LIMIT,
    TRANSPORT_TX_ERROR
} transport_tx_result;

transport_rx_result transport_rx(byte* buffer, uint16_t buf_len, uint16_t* message_len, byte* source_port, uint16_t timeout_ms);

transport_tx_result transport_tx(byte* message, uint16_t message_len, byte dest_port);

#endif

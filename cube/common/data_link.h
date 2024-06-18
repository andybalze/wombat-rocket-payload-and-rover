#ifndef _DATA_LINK_H
#define _DATA_LINK_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

typedef enum {
    DATA_LINK_RX_SUCCESS,
    DATA_LINK_RX_TIMEOUT,
    DATA_LINK_RX_ERROR
} data_link_rx_result;

typedef enum {
    DATA_LINK_TX_SUCCESS,
    DATA_LINK_TX_FAILURE
} data_link_tx_result;

bool data_link_push_to_fifo(byte* buffer, byte buf_len);

data_link_rx_result data_link_rx(byte* buffer, byte buf_len, uint16_t timeout_ms);

data_link_tx_result data_link_tx(byte* payload, byte payload_len, uint32_t addr);

#endif
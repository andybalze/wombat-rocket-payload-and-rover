#ifndef _NETWORK_H
#define _NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

typedef enum {
    NETWORK_RX_SUCCESS,
    NETWORK_RX_TIMEOUT,
    NETWORK_RX_ERROR
} network_rx_result;

typedef enum {
    NETWORK_TX_SUCCESS,
    NETWORK_TX_FAILURE
} network_tx_result;

// This function blocks and waits until it receives a packet
// destined for us. It might even forward packets while waiting.
// How fun!
network_rx_result network_rx(byte* buffer, byte buf_len, uint16_t timeout_ms);

network_tx_result network_tx(byte* payload, byte payload_len, byte dest_network_addr, byte src_network_addr);

#endif

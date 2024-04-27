#ifndef _NETWORK_H
#define _NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

// This function blocks and waits until it receives a packet
// destined for us. It might even forward packets while waiting.
// How fun!
bool network_rx(byte* buffer, byte buf_len, uint16_t timeout_ms);

bool network_tx(byte* payload, byte payload_len, byte dest_network_addr, byte src_network_addr);

#endif

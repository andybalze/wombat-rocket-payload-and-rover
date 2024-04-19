#ifndef _DATA_LINK_H
#define _DATA_LINK_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

bool data_link_push_to_fifo(byte* buffer, byte buf_len);

byte data_link_rx(byte* buffer, byte buf_len);

void data_link_tx(byte* payload, byte payload_len, uint32_t addr);

#endif
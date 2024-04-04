#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

// This function receives a message and stores it in the buffer.
// This is a blocking function that will only return when
// it has received data.
// The return value is the length of the message.
byte transport_rx(byte* buffer, byte buf_len);

void transport_tx(byte* message, byte message_len, byte dest_port);

#endif

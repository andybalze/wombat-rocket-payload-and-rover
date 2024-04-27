#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#include <stdint.h>
#include <stdbool.h>
#include "networking_constants.h"

bool transport_rx(byte* buffer, byte buf_len);

bool transport_tx(byte* message, byte message_len, byte dest_port);

#endif

#ifndef _LOG_H
#define _LOG_H

#include <stdint.h>
#include "networking_constants.h"

// Store a message in the EEPROM.
// To avoid excessive EEPROM writes, this function stores messages
// similar to a circular buffer.
void log_message(byte* message, uint16_t message_len, uint8_t message_source);

// Print the last three stored messages.
void print_log();

// Completely erase the log.
void init_log();

#endif

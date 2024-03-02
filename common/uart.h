#ifndef _UART_H
#define _UART_H

////////////////////////////////////////////////////////////////////////////////
//
// UART
//
// Provides functions that allow designs to use the ATMega328p's U(S)ART
// peripheral.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

///////////////////// UART Settings ////////////////////////////////////////////

#define UART_MESSAGE_MAX_LENGTH (256)

///////////////////// Type Definitions /////////////////////////////////////////

// An element of a UART message.
typedef char uart_message_element_t;

// Indexes into a uart message.
typedef uint8_t uart_message_index_t;

// The length of a uart message.
typedef uint8_t uart_message_length_t;

///////////////////// Public Function Prototypes ///////////////////////////////

// Initializes the U(S)ART, including configuring the appropriate pins.
void uart_initialize(void);

// Transmits a message over the U(S)ART. Returns 1 if the message can be
// transmitted, 0 otherwise (if another message is currently being transmitted).
uint8_t uart_transmit_message(
  const uart_message_element_t *message, 
  int length
);

// Transmits a formatted message over the U(S)ART. Returns the number of
// characters that will be transmitted. If the message cannot be transmitted at
// this time, returns 0. If the message to be transmitted cannot fit in the
// message buffer, transmits as many characters as possible and discards the
// rest.
void uart_transmit_formatted_message(
  uart_message_element_t *message_format,
  ...
);

#endif
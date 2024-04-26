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

/////////////////// SPI macros /////////////////////////////////////////////////

// Waits until the UART message currently being transmitted has finished.
#define UART_WAIT_UNTIL_DONE() while((UCSR0B & _BV(UDRIE0)) != 0)

///////////////////// Type Definitions /////////////////////////////////////////

// An element of a UART message.
typedef uint8_t uart_message_element_t;

// Indexes into a uart message.
typedef uint16_t uart_message_index_t;

// The length of a uart message.
typedef uint16_t uart_message_length_t;

///////////////////// Public Function Prototypes ///////////////////////////////

// Initializes the U(S)ART, including configuring the appropriate pins.
void uart_initialize(void);

// Transmits a formatted message over the U(S)ART. Returns the number of
// characters that will be transmitted. If the message cannot be transmitted at
// this time, returns 0. If the message to be transmitted cannot fit in the
// message buffer, transmits as many characters as possible and discards the
// rest.
uart_message_length_t uart_transmit_formatted_message(
  const uart_message_element_t *message_format,
  ...
);

#endif
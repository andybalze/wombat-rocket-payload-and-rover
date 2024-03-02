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

#include <stdarg.h>
#include <stdio.h>

#include "uart.h"

//////////////// Static Variable Definitions ///////////////////////////////////

// The message that is currently being transmitted.
static uart_message_element_t message_buffer[UART_MESSAGE_MAX_LENGTH];

// The index of the final element in the current message. Once this element has
// been transmitted, the message has been fully transmitted.
static uart_message_index_t message_index_final_element;

// The element that the U(S)ART will transmit next.
static uart_message_index_t message_index_current_element;

//////////////// Public Function Bodies ////////////////////////////////////////

// Initializes the U(S)ART, including configuring the appropriate pins.
void uart_initialize(void) {

  // Disables global interrupts until initialization is complete.
  SREG &= ~_BV(SREG_I);

  // Initialize the USART registers.
  
  UCSR0B =
    _BV(RXCIE0) | // Enables data receive interrupt.
    _BV(RXEN0)  | // Enables transmitter.
    _BV(TXEN0)    // Enables receiver.
  ;

  UCSR0C =
    _BV(UCSZ01) | // UART characters are 8 bits.
    _BV(UCSZ00) 
  ;

  UBRR0 = 51;    // Select a baud rate of 9600 (Assuming 8MHz clock).

  // Initialize the appropriate pins.
  // I don't think there's any pin configuration to do??


  // Reenables global interrupts.
  SREG |= _BV(SREG_I);

}

// Deprecated. Use uart_transmit_formatted_message instead.
uint8_t uart_transmit_message(
  const uart_message_element_t *message, 
  int length
) {

  uart_message_element_t message_length;
  message_length = uart_transmit_formatted_message(message);

  if (message_length == 0) {
    return 0;
  } else {
    return 1;
  }

}

// Transmits a formatted message over the U(S)ART. Returns the number of
// characters that will be transmitted. If the message cannot be transmitted at
// this time, returns 0. If the message to be transmitted cannot fit in the
// message buffer, transmits as many characters as possible and discards the
// rest.
uart_message_length_t uart_transmit_formatted_message(
  uart_message_element_t *message_format,
  ...
) {

  va_list args;
  va_start(args, message_format);
  
  // If a message is already being transmitted, a new one cannot be started.
  // The status is determined by checking the interrupt enable bit.
  int interrupt_enabled;
  interrupt_enabled = UCSR0B & _BV(UDRIE0);
  if (interrupt_enabled != 0) {
    return 0;
  }

  // Prints the formatted message into the uart message buffer.
  int formatted_character_count;
  formatted_character_count = vsnprintf(
    message_buffer, 
    UART_MESSAGE_MAX_LENGTH, 
    message_format, 
    args
  );

  // Determine the possibly-truncated length of the message.
  int message_length;
  if(formatted_character_count > UART_MESSAGE_MAX_LENGTH) {
    message_length = UART_MESSAGE_MAX_LENGTH;
  } else {
    message_length = formatted_character_count;
  }

  message_index_final_element = message_length - 1;

  va_end(args);

  // Begin the transmission.
  message_index_current_element = 0;
  UDR0 = message_buffer[0];
  UCSR0B |= _BV(UDRIE0);

  return message_length;

}

///////////// Interrupt Service Routines ///////////////////////////////////////

// Transmit data empty interrupt handler. Either transmits the next character or
// updates static variables to reflect that the message has finished.
ISR(USART_UDRE_vect) {

  // If that was the final element, the message has finished.
  if (message_index_current_element == message_index_final_element) {
    UCSR0B &= ~_BV(UDRIE0); // Disable the interrupt
    return;
  }

  message_index_current_element++;
  UDR0 = message_buffer[message_index_current_element];

}

// The user must provide their own data received handler.
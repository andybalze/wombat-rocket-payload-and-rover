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

uint8_t uart_transmit_message(
  const uart_message_element_t *message, 
  int length
) {

  // If a message is already being transmitted, a new one cannot be started.
  // The status is determined by checking the interrupt enable bit.
  int interrupt_enabled;
  interrupt_enabled = UCSR0B & _BV(UDRIE0);
  if (interrupt_enabled != 0) {
    return 0;
  }

  // If the message's length is larger than the maximum allowed, the message
  // cannot be transmitted.
  if (length > UART_MESSAGE_MAX_LENGTH) {
    return 0;
  }

  message_index_final_element = length - 1;

  // Copy the message into the message buffer.
  int i;
  for (i = 0; i < length; i++) {
    message_buffer[i] = *(message + i);
  }

  // Start at the beginning of the message.
  message_index_current_element = 0;
  UDR0 = message_buffer[0];

  // The message's transmission has been started successfully.
  UCSR0B |= _BV(UDRIE0);  // Enable the interrupt
  return 1;
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
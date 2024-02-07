////////////////////////////////////////////////////////////////////////////////
//
// Main
//
// All the code necessary for a program that simply toggles an LED on and off
// at a rate of 1 Hz. Intended as a "Hello World" program to provide a starting
// point for more sophisticated projects.
//
// This project assumes the anode (+) of an LED connected to Pin PB0.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>>

int main(void) {

  // Configure the chosen pin as an output.
  DDRB |= _BV(DDB0);
  
}

ISR(TIMER0_COMPA_vect) {

  // Toggles the LED state.
  PORTB ^= _BV(PORTB0);

}
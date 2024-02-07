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
#include <avr/interrupt.h>

int main(void) {

  // Configure the chosen pin as an output.
  DDRB |= _BV(DDB0);

  // Configure Timer1 to generate interrupts.
  
  // Selects the 1/1024 prescaler.
  TCCR1B |= _BV(CS10);

  // Sets the timer into CTC mode.
  TCCR1B |= _BV(WGM12);

  // Sets the output compare value for turning off the LED.
  OCR1A = 15624; // 16,000,000/1024 - 1 = 15,624

  // Sets the output compare value for turning on the LED.
  // The LED turns on halfway through the counter cycle.
  OCR1B = 7812;

  // Enables the output compare A and B interrupts
  TIMSK1 |= _BV(OCIE1A);
  TIMSK1 |= _BV(OCIE1B);

  // Enables global interrupts.
  SREG |= _BV(SREG_I);

  // Loops forever at the end of main.
  while(1);

}

ISR(TIMER1_COMPB_vect) {

  // Turns on the LED.
  PORTB |= _BV(PORTB0);

}

ISR(TIMER1_COMPA_vect) {

  // Turns off the LED.
  PORTB &= ~_BV(PORTB0);

}
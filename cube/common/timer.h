////////////////////////////////////////////////////////////////////////////////
//
// Timer
//
// Provides functions for using the ATMega's 16-bit Timer/Counter 1 for basic
// timing delays in situations that do not require interrupts.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _TIMER_H
#define _TIMER_H

#include <avr/io.h>

/////////////////// Timer Macros ///////////////////////////////////////////////

// Whether the timer has "gone off".
#define TIMER_DONE ((TIFR1 & _BV(OCF1A)) != 0)

/////////////////// Timer Type Definitions /////////////////////////////////////

// How many milliseconds to wait for an event.
typedef uint16_t timer_delay_ms_t;

/////////////////// Public Function Prototypes /////////////////////////////////

// Starts the timer for a certain number of milliseconds.
void timer_start(timer_delay_ms_t delay_ms);

// Stops the timer.
void timer_stop(void);

#endif
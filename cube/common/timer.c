////////////////////////////////////////////////////////////////////////////////
//
// Timer
//
// Provides functions for using the ATMega's 16-bit Timer/Counter 1 for basic
// timing delays in situations that do not require interrupts.
//
////////////////////////////////////////////////////////////////////////////////

#include "timer.h"

#include <avr/io.h>

/////////////////// Public Function Bodies /////////////////////////////////////

void timer_start(timer_delay_ms_t delay_ms) {

  // Sets the timer count to 0.
  TCNT1 = 0;

  // Loads the delay value into the compare register.
#if TIMER_CPU_FREQUENCY == 1000000
  OCR1A = delay_ms;
#elif TIMER_CPU_FREQUENCY == 8000000
  OCR1A = delay_ms << 3;
#else
  #error "TIMER_CPU_FREQUENCY has invalid value."
#endif

  // Starts running the timer in CTC-OCR1A mode from the 1/1024 prescaler.
	TCCR1B = (_BV(WGM12) | _BV(CS12) | _BV(CS10));

}

void timer_stop(void) {
	TIFR1 |= _BV(OCF1A);
	TCCR1B = 0;
}
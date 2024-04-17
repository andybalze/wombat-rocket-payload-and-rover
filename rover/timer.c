#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"
#include "digital_io.h"

void timer_initialize(void) {
    TCCR0B |= _BV(CS02) | _BV(CS00);    // Select the 1024 prescaler
    TCCR0A |= _BV(WGM01);               // Set timer to CTC mode
    TIMSK0 |= _BV(OCIE0A);              // Enable output compare channel A interrupt
    // TIMSK0 |= _BV(OCIE0B);           // Enable output compare channel B interrupt

    OCR0A = 255;
    // OCR0B = 

    SREG   |= _BV(SREG_I);              // Enable global interrupts
}

ISR(TIMER0_COMPA_vect) {    // 30.6 Hz interrupt frequency
    
}
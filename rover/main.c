////////////////////////////////////////////////////////////////////////////////
//
// Main
//
// 
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#define R1_DDR      DDRD
#define R1_PORT     PORTD
#define R1_INDEX    PIN3
#define R1_OCR      OCR2B

#define R2_DDR      DDRB
#define R2_PORT     PORTB
#define R2_INDEX    PIN3
#define R2_OCR      OCR2A

#define Y1_DDR      DDRD
#define Y1_PORT     PORTD
#define Y1_INDEX    PIN6
#define Y1_OCR      OCR0A

#define Y2_DDR      DDRD
#define Y2_PORT     PORTD
#define Y2_INDEX    PIN5
#define Y2_OCR      OCR0B

#define G1_DDR      DDRB
#define G1_PORT     PORTB
#define G1_INDEX    PIN1
#define G1_OCR      OCR1A

#define G2_DDR      DDRB
#define G2_PORT     PORTB
#define G2_INDEX    PIN2
#define G2_OCR      OCR1B


int main(void) {

    // Set data direction for output pins
    R1_DDR |= _BV(R1_INDEX);
    R2_DDR |= _BV(R2_INDEX);
    G1_DDR |= _BV(G1_INDEX);
    G2_DDR |= _BV(G2_INDEX);
    Y1_DDR |= _BV(Y1_INDEX);
    Y2_DDR |= _BV(Y2_INDEX);

    // Initialize outputs as off
    R1_PORT &= ~_BV(R1_INDEX);
    R2_PORT &= ~_BV(R2_INDEX);
    G1_PORT &= ~_BV(G1_INDEX);
    G2_PORT &= ~_BV(G2_INDEX);
    Y1_PORT &= ~_BV(Y1_INDEX);
    Y2_PORT &= ~_BV(Y2_INDEX);

    // Configure Timers0-2 for PWM.

    // // Select the 1/1024 prescaler for all timers
    // TCCR0B |= _BV(CS00) | _BV(CS02);
    // TCCR1B |= _BV(CS10) | _BV(CS12);
    // TCCR2B |= _BV(CS20) | _BV(CS21) | _BV(CS22);

    // Select no prescaler for all the timers
    TCCR0B |= _BV(CS00);
    TCCR1B |= _BV(CS10);
    TCCR2B |= _BV(CS20);

    // Set waveform generation mode to fast PWM (fast PWM is up-counting only)
    TCCR0A |= _BV(WGM00) | _BV(WGM01);
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(WGM12);
    TCCR2A |= _BV(WGM20) | _BV(WGM21);

    // Set compare match behavior to clear on compare match
    TCCR0A |= _BV(COM0A1); //R1
    TCCR1A |= _BV(COM1A1); //G1
    TCCR2A |= _BV(COM2A1); //Y2
    TCCR0A |= _BV(COM0B1); //R2
    TCCR1A |= _BV(COM1B1); //G2
    TCCR2A |= _BV(COM2B1); //Y1

    // Set the duty cycle 0-0xFF (0-255)
    R1_OCR = 50;
    Y1_OCR = 50;
    G1_OCR = 75;
    R2_OCR = 100;
    Y2_OCR = 100;
    G2_OCR = 100;


    // // Enables the output compare A and B interrupts
    // TIMSK1 |= _BV(OCIE1A);
    // TIMSK1 |= _BV(OCIE1B);

    // // Enables global interrupts.
    // SREG |= _BV(SREG_I);

    // Loops forever at the end of main.
    while(1);

}

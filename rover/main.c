////////////////////////////////////////////////////////////////////////////////
//
// Main
//
// All the code necessary for a program that simply toggles an LED on and off
// at a rate of 1 Hz. Intended as a "Hello World" program to provide a starting
// point for more sophisticated projects.
//
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_DDR   DDRB
#define LED_PORT  PORTB
#define LED0_INDEX PIN0
#define LED1_INDEX PIN5

int main(void) {

    // Configure the chosen pin as an output.
    LED_DDR |= _BV(LED0_INDEX);
    LED_DDR |= _BV(LED1_INDEX);

    // Initialize both LEDs as off
    LED_PORT |= _BV(LED0_INDEX);
    LED_PORT |= _BV(LED1_INDEX);

    // Configure Timer1 to generate interrupts.

    // Selects the 1/1024 prescaler.
    TCCR1B |= _BV(CS10);
    TCCR1B |= _BV(CS12);

    // Sets the timer into CTC mode.
    TCCR1B |= _BV(WGM12);

    // Sets the output compare value for turning off the LED.
    OCR1A = 975; // 1,000,000/1024 - 1 = 975

    // Sets the output compare value for turning on the LED.
    // The LED turns on halfway through the counter cycle.
    OCR1B = 487;

    // Enables the output compare A and B interrupts
    TIMSK1 |= _BV(OCIE1A);
    TIMSK1 |= _BV(OCIE1B);

    // Enables global interrupts.
    SREG |= _BV(SREG_I);

    // Loops forever at the end of main.
    while(1);

}

// Handles the interrupt generated whenever the count register of Timer 1
// equals the value in register OCR1B. In this case, that means turning the
// LED on.
ISR(TIMER1_COMPB_vect) {

    static int decide_LED = 0;

    if (decide_LED == 0)
    {
        LED_PORT &= ~_BV(LED0_INDEX);   // Turn on LED0
        LED_PORT |= _BV(LED1_INDEX);    // Turn off LED1
        decide_LED = 1;
    }
    else // if (decide_LED == 1)
    {
        LED_PORT |= _BV(LED0_INDEX);    // Turn off LED0
        LED_PORT &= ~_BV(LED1_INDEX);   // Turn on LED1
        decide_LED = 0;
    }

}

// Handles the interrupt generated whenever the count register of Timer 1
// equals the value in register OCR1A. In this case, that means turning the
// LED off.
ISR(TIMER1_COMPA_vect) {

    // Turns off both LEDs.
    LED_PORT |= _BV(LED0_INDEX);
    LED_PORT |= _BV(LED1_INDEX);

}
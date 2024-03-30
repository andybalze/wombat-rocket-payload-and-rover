////////////////////////////////////////////////////////////////////////////////
//
// Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

// Macros for LEDs
#define LED0_DDR   DDRB
#define LED0_PORT  PORTB
#define LED0_INDEX PIN0

#define LED1_DDR   DDRB
#define LED1_PORT  PORTB
#define LED1_INDEX PIN5


// Macros for switch & pushbuttons
#define SW2_DDR     DDRD
#define SW2_PORT    PORTD   //config pull-up
#define SW2_RD      PIND
#define SW2_INDEX   PIN2
//set an internal pull-up resistor?

#define SW3_DDR     DDRB
#define SW3_PORT    PORTB   //config pull-up
#define SW3_RD      PINB
#define SW3_INDEX   PIN6

#define SW4_DDR     DDRD
#define SW4_PORT    PORTD   //config pull-up
#define SW4_RD      PIND
#define SW4_INDEX   PIN7


int main(void) {

    int sw2State, sw3State, sw4State;

    // Configure the LED pins as outputs
    LED0_DDR |= _BV(LED0_INDEX);
    LED1_DDR |= _BV(LED1_INDEX);

    // Initialize both LEDs as off
    LED0_PORT |= _BV(LED0_INDEX);
    LED1_PORT |= _BV(LED1_INDEX);

    // Configure the switch & pushbuttons as inputs
    SW2_DDR &= ~_BV(SW2_INDEX);
    SW3_DDR &= ~_BV(SW3_INDEX);
    SW4_DDR &= ~_BV(SW4_INDEX);

    // Configure the switch & pushbuttons' pull-up resistors
    SW2_PORT |=  _BV(SW2_INDEX);    // Turn on pull-up resistor
    SW3_PORT &= ~_BV(SW3_INDEX);    // Turn off pull-up resistor
    SW4_PORT &= ~_BV(SW4_INDEX);    // Turn off pull-up resistor

    while(1)
    {
        sw2State = (SW2_RD & _BV(SW2_INDEX)) >> SW2_INDEX;
        sw3State = (SW3_RD & _BV(SW3_INDEX)) >> SW3_INDEX;
        sw4State = (SW4_RD & _BV(SW4_INDEX)) >> SW4_INDEX;


        if (sw2State ^ sw3State)
        {
            LED0_PORT &= ~_BV(LED0_INDEX);          // turn LED0 on
        }
        else
        {
            LED0_PORT |= _BV(LED0_INDEX);           // turn LED0 off
        }

        if (sw2State ^ sw4State)
        {
            LED1_PORT &= ~_BV(LED1_INDEX);          // turn LED1 on
        }
        else
        {
            LED1_PORT |= _BV(LED1_INDEX);           // turn LED1 off
        }
    }

    // Loop forever at the end of main
    while(1);

}
#include "digital_io.h"

#include "cube_parameters.h"
#include <util/delay.h>

// Macros for LEDs
#define RED_DDR     DDRD
#define RED_PORT    PORTD
#define RED_INDEX   PIN6

#define GREEN_DDR   DDRD
#define GREEN_PORT  PORTD
#define GREEN_INDEX PIN5

#define BLUE_DDR   DDRC
#define BLUE_PORT  PORTC
#define BLUE_INDEX PIN0

#define BLINK_DURATION_MS (25)


// Macros for switch & pushbuttons
#define SW1_DDR     DDRD
#define SW1_PORT    PORTD   //config pull-up
#define SW1_RD      PIND
#define SW1_INDEX   PIN3


void digital_io_initialize(void) {
    // Configure the LED pins as outputs
    RED_DDR   |= _BV(RED_INDEX);
    GREEN_DDR |= _BV(GREEN_INDEX);
    BLUE_DDR  |= _BV(BLUE_INDEX);

    // Initialize both LEDs as off
    RED_PORT   |= _BV(RED_INDEX);
    GREEN_PORT |= _BV(GREEN_INDEX);
    BLUE_DDR   |= _BV(BLUE_INDEX);

    // Configure the limit switch as an input
    SW1_DDR &= ~_BV(SW1_INDEX);

    // Configure the switch & pushbuttons' pull-up resistors
    SW1_PORT &= ~_BV(SW1_INDEX);    // Turn off pull-up resistor because we have a hardware pull-up
}

void LED_set_state(char state) {
    if (((state >> 2) & 1) == 1) {      // if (left bit == 1)
        RED_PORT &= ~_BV(RED_INDEX);
    }
    else {                              // if (left bit == 0)
        RED_PORT |= _BV(RED_INDEX);
    }

    if (((state >> 1) & 1) == 1) {      // if (middle bit == 1)
        GREEN_PORT &= ~_BV(GREEN_INDEX);
    }
    else {                              // if (middle bit == 0)
        GREEN_PORT |= _BV(GREEN_INDEX);
    }

    if (((state >> 0) & 1) == 1) {      // if (right bit == 1)
        BLUE_PORT &= ~_BV(BLUE_INDEX);
    }
    else {                              // if (right bit == 0)
        BLUE_PORT |= _BV(BLUE_INDEX);
    }
}



char SW_read(SW_name_t sw) {
    char return_val;

    switch (sw) {
        case SW1: {
            return_val = (SW1_RD & _BV(SW1_INDEX)) >> SW1_INDEX;
            break;
        }
    }

    return return_val;
}

// Done so LED can flash blue when receiving, and return to this color
// after it is done.
static char LED_color = LED_OFF;

void LED_set(char color) {
    LED_color = color;
    LED_set_state(color);
}

void LED_blink(char color) {
    if (color == LED_color) {
        LED_set_state(LED_OFF);
        _delay_ms(BLINK_DURATION_MS);
        LED_set_state(LED_color);
        _delay_ms(BLINK_DURATION_MS);
    }
    else {
        LED_set_state(color);
        _delay_ms(BLINK_DURATION_MS);
        LED_set_state(LED_color);
        _delay_ms(BLINK_DURATION_MS);
    }
    return;
}

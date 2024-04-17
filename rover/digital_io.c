#include "digital_io.h"

// Macros for LEDs
#define RED_DDR     DDRB
#define RED_PORT    PORTB
#define RED_INDEX   PIN0

#define GREEN_DDR   DDRB
#define GREEN_PORT  PORTB
#define GREEN_INDEX PIN5


// Macros for switch & pushbuttons
#define SW2_DDR     DDRD
#define SW2_PORT    PORTD   //config pull-up
#define SW2_RD      PIND
#define SW2_INDEX   PIN2

#define SW3_DDR     DDRB
#define SW3_PORT    PORTB   //config pull-up
#define SW3_RD      PINB
#define SW3_INDEX   PIN6

#define SW4_DDR     DDRD
#define SW4_PORT    PORTD   //config pull-up
#define SW4_RD      PIND
#define SW4_INDEX   PIN7

void digital_io_initialize(void) {
    // Configure the LED pins as outputs
    RED_DDR   |= _BV(RED_INDEX);
    GREEN_DDR |= _BV(GREEN_INDEX);

    // Initialize both LEDs as off
    RED_PORT   |= _BV(RED_INDEX);
    GREEN_PORT |= _BV(GREEN_INDEX);

    // Configure the switch & pushbuttons as inputs
    SW2_DDR &= ~_BV(SW2_INDEX);
    SW3_DDR &= ~_BV(SW3_INDEX);
    SW4_DDR &= ~_BV(SW4_INDEX);

    // Configure the switch & pushbuttons' pull-up resistors
    SW2_PORT |=  _BV(SW2_INDEX);    // Turn on pull-up resistor
    SW3_PORT &= ~_BV(SW3_INDEX);    // Turn off pull-up resistor
    SW4_PORT &= ~_BV(SW4_INDEX);    // Turn off pull-up resistor
}

void LED_set(LED_color_t color, LED_state_t state) {
    switch (color) {
        case RED: {
            switch (state) {
                case ON: {
                    RED_PORT &= ~_BV(RED_INDEX);
                    break;
                }

                case OFF: {
                    RED_PORT |= _BV(RED_INDEX);
                    break;
                }

                default: {
                    break;
                }
            }
            break;
        }

        case GREEN: {
            switch (state) {
                case ON: {
                    GREEN_PORT &= ~_BV(GREEN_INDEX);
                    break;
                }

                case OFF: {
                    GREEN_PORT |= _BV(GREEN_INDEX);
                    break;
                }

                default: {
                    break;
                }
            }
            break;
        }

        default: {
            break;
        }
    }
}



char SW_read(SW_name_t sw) {
    char return_val;

    switch (sw) {
        case SW2: {
            return_val = (SW2_RD & _BV(SW2_INDEX)) >> SW2_INDEX;
            break;
        }

        case SW3: {
            return_val = (SW3_RD & _BV(SW3_INDEX)) >> SW3_INDEX;
            break;
        }

        case SW4: {
            return_val = (SW4_RD & _BV(SW4_INDEX)) >> SW4_INDEX;
            break;
        }

        default: {
            break;
        }
    }

    return return_val;
}
#include "test.h"

// Macros for LEDs
#define RED_DDR     DDRB
#define RED_PORT    PORTB
#define RED_INDEX   PIN0

#define GREEN_DDR   DDRB
#define GREEN_PORT  PORTB
#define GREEN_INDEX PIN5


void digital_io_initialize(void) {
    // Configure the LED pins as outputs
    RED_DDR   |= _BV(RED_INDEX);
    GREEN_DDR |= _BV(GREEN_INDEX);

    // Initialize both LEDs as off
    RED_PORT   |= _BV(RED_INDEX);
    GREEN_PORT |= _BV(GREEN_INDEX);
}

void LED_set(char state) {
    if (((state >> 1) & 1) == 1) {        // if (left bit == 1)
        RED_PORT &= ~_BV(RED_INDEX);
    }
    else {                          // if (left bit == 0)
        RED_PORT |= _BV(RED_INDEX);
    }

    if (((state >> 0) & 1) == 1) {        // if (right bit == 1)
        GREEN_PORT &= ~_BV(GREEN_INDEX);
    }
    else {                          // if (right bit == 0)
        GREEN_PORT |= _BV(GREEN_INDEX);
    }
}

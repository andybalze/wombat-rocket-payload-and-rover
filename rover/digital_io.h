#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <avr/io.h>

enum LED_color_enum {
    RED = 0,
    GREEN = 1,
    BLUE = 2    // No BLUE on the rover
};
typedef enum LED_color_enum LED_color_t;

enum LED_state_enum {
    ON  = 0,  // The LEDs are active-low
    OFF = 1
};
typedef enum LED_state_enum LED_state_t;

enum SW_name_enum {
    SW2 = 2,
    SW3 = 3,
    SW4 = 4,
    ROVER_MODE_SW = 2,
    LOAD_BTN = 3,
    UNLOAD_BTN = 4
};
typedef enum SW_name_enum SW_name_t;

void digital_io_initialize(void);
void LED_set(LED_color_t color, LED_state_t state);
char SW_read(SW_name_t sw);

#endif //DIGITAL_IO_H

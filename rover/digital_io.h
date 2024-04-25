#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <avr/io.h>

enum LED_color_enum {
    RED,
    GREEN,
    YELLOW
};
typedef enum LED_color_enum LED_color_t;

enum output_state_enum {
    ON  = 1,  // Note: the LEDs and data cube signal are active-low, but it is switched here for user readability
    OFF = 0,
};
typedef enum output_state_enum output_state_t;

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
void LED_set(LED_color_t color, output_state_t state);
void signal_data_cube(output_state_t state);
char SW_read(SW_name_t sw);

#endif //DIGITAL_IO_H

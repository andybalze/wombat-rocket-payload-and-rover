#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <avr/io.h>

//                   red        green      blue
#define LED_OFF     ((0 << 2) | (0 << 1) | (0 << 0))
#define LED_BLUE    ((0 << 2) | (0 << 1) | (1 << 0))
#define LED_GREEN   ((0 << 2) | (1 << 1) | (0 << 0))
#define LED_CYAN    ((0 << 2) | (1 << 1) | (1 << 0))
#define LED_RED     ((1 << 2) | (0 << 1) | (0 << 0))
#define LED_MAGENTA ((1 << 2) | (0 << 1) | (1 << 0))
#define LED_YELLOW  ((1 << 2) | (1 << 1) | (0 << 0))
#define LED_WHITE   ((1 << 2) | (1 << 1) | (1 << 0))

enum SW_name_enum {
    SW1 = 1
};
typedef enum SW_name_enum SW_name_t;

void digital_io_initialize(void);
void LED_set(char state);     //Enter a value between 0-7 where the binary representation corresponds to RGB encoding. Ex: 010 would be green
char SW_read(SW_name_t sw);

#endif //DIGITAL_IO_H

#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <avr/io.h>

enum SW_name_enum {
    SW1 = 1
};
typedef enum SW_name_enum SW_name_t;

void digital_io_initialize(void);
void LED_set(char state);     //Enter a value between 0-7 where the binary representation corresponds to RGB encoding. Ex: 010 would be green
char SW_read(SW_name_t sw);

#endif //DIGITAL_IO_H

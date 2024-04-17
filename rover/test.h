#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#include <avr/io.h>

void digital_io_initialize(void);
void LED_set(char state);

#endif //DIGITAL_IO_H

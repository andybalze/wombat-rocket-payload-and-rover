#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "digital_io.h"
#include "uart.h"

enum timer_name_enum {
    timerA,
    timerB
};
typedef enum timer_name_enum timer_name_t;

void timer_initialize(void);
void reset_timer(timer_name_t timer);
unsigned long int get_timer_cnt(timer_name_t timer);

#endif // TIMER_H
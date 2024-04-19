#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "digital_io.h"
#include "uart.h"

#define ONE_SECOND 1000
#define ONE_MINUTE 60000

enum timer_name_enum {
    timer_alpha,
    timer_beta
};
typedef enum timer_name_enum timer_name_t;

void timer_initialize(void);
void reset_timer(timer_name_t timer);
uint32_t get_timer_cnt(timer_name_t timer);

#endif // TIMER_H
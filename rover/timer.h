#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "digital_io.h"
#include "uart.h"
#include "accelerometer.h"

#define ONE_SECOND 1000
#define ONE_MINUTE 60000


enum counter_name_enum {
    counter_alpha,
    counter_beta
};
typedef enum counter_name_enum counter_name_t;


void timer_initialize(void);
void reset_timer_counter(counter_name_t counter);
uint32_t get_timer_counter(counter_name_t counter);

// Enables timer0 channel b interrupt
void enable_launch_check(void);


// Disables timer0 channel b interrupt
void disable_launch_check(void);

#endif // TIMER_H

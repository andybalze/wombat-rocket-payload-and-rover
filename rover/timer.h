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


// Initializes TIMER0 which is used for two seperate counters. Can be used at the same time as Left and Right motor PWM
void timer_counter_initialize(void);
void reset_timer_counter(counter_name_t counter);
uint32_t get_timer_counter(counter_name_t counter);

// Enables timer2 channel a interrupt. Cannot function if PWM_enable (from motors.c) is called
void launch_check_enable(void);

// Disables timer2 channel a interrupt
void launch_check_disable(void);

#endif // TIMER_H

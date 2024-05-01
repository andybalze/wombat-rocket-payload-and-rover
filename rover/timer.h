#ifndef TIMER_H
#define TIMER_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "digital_io.h"
#include "uart.h"
#include "accelerometer.h"

#define ONE_SECOND (1000)
#define ONE_MINUTE (60000)


typedef uint64_t logic_vector;

enum counter_name_enum {
    counter_alpha,
    counter_beta
};
typedef enum counter_name_enum counter_name_t;


// Initializes TIMER0 which is used for two seperate counters. Can be used at the same time as Left and Right motor PWM
// Cannot be used while either launch_check or no_motion_check are enabled.
void timer_counter_initialize(void);
void reset_timer_counter(counter_name_t counter);
uint32_t get_timer_counter(counter_name_t counter);

// Enables timer2 channel A interrupt. Cannot function if either PWM_enable (from motors.c) no_motion_check_enable are called
// Cannot use the timer counter while launch check is enabled
void launch_check_enable(void);

// Disables timer2 channel A interrupt
void launch_check_disable(void);

// Enables timer2 channel B interrupt. Cannot function if either PWM_enable (from motors.c) or launch_check_enable are called
void no_motion_check_enable(void);

// Disables timer2 channel B interrupt
void no_motion_check_disable(void);

#endif // TIMER_H

#ifndef MOTORS_H
#define MOTORS_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "digital_io.h"
#include "uart.h"

#define SPEED_MAX 249

enum motor_direction_enum {
    REVERSE = 0,
    FORWARD = 1
};
typedef enum motor_direction_enum motor_direction_t;

enum motor_name_enum {
    LEFT_MOTOR,
    RIGHT_MOTOR,
    DISPENSER_MOTOR,
};
typedef enum motor_name_enum motor_name_t;


// Initializes motor IO pins. PWM must be initialized seperately
void motors_initialize(void);

// Enables PWM for left and right (but not dispenser) motors. Cannot function if PWM_enable (from motors.c) is called
void PWM_enable(void);

// Sets the speed and direction of a specified motor. Dispenser to dispense is forward
void motor(motor_name_t motor_name, motor_direction_t direction, uint8_t speed);

#endif //MOTORS_H
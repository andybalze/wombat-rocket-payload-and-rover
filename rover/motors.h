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

_Noreturn void rover_failure_state(void);

void motors_initialize(void);
void motor(motor_name_t motor_name, motor_direction_t direction, uint8_t speed);

#endif //MOTORS_H

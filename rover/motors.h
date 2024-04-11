#ifndef MOTORS_H
#define MOTORS_H

#include <avr/io.h>
#include <avr/interrupt.h>

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

void motors_initialize(void);
void motor(motor_name_t motor_name, motor_direction_t direction, char speed);

#endif //MOTORS_H

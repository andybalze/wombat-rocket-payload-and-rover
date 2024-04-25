#include "ir.h"
#include "motors.h"


void avoid(void) {
    motor(RIGHT_MOTOR, FORWARD, SPEED_MAX);
        
        if (ir_distance_read() > 26) {
            motor(LEFT_MOTOR, FORWARD, (uint8_t)(8 * (ir_distance_read()-25)));
        }
        else if (ir_distance_read() < 24) {
            motor(LEFT_MOTOR, REVERSE, (uint8_t)(8 * (25-ir_distance_read())));
        }
        else {
            motor(LEFT_MOTOR, FORWARD, 0);
        }
    return;
}
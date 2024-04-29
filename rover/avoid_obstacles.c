#include "avoid_obstacles.h"
#include "ir.h"
#include "motors.h"


void avoid(bool is_upside_down) {
    motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
        
        if (ir_distance_read() > 26) {
            motor(LEFT_MOTOR, FORWARD ^ is_upside_down, (uint8_t)(8 * (ir_distance_read()-25)));
        }
        else if (ir_distance_read() < 24) {
            motor(LEFT_MOTOR, REVERSE ^ is_upside_down, (uint8_t)(8 * (25-ir_distance_read())));
        }
        else {
            motor(LEFT_MOTOR, FORWARD ^ is_upside_down, 0);
        }
    return;
}
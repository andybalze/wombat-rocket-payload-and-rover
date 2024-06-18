#include "avoid_obstacles.h"
#include "ir.h"
#include "motors.h"
#include "timer.h"

// Distance in cm the left tread goes from forward to stop. At two less cm, it switches to rev
#define FORW_STOP_REV 30

// Time the rover continues to turn after an object has left its field of view
#define KEEP_TURNING (2 * ONE_SECOND)


void avoid(bool is_upside_down) {
    motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);

    static uint64_t count = 0;
    static bool obstacle = false;
    int16_t ir_val;
    uint32_t counter_val;

    ir_val = ir_distance_read();
    counter_val = get_timer_counter(counter_beta);

    if (ir_val > FORW_STOP_REV) {
        motor(LEFT_MOTOR, FORWARD ^ is_upside_down, (uint8_t)(8 * (ir_distance_read()-25)));
    }
    else if (ir_val < FORW_STOP_REV - 2) {
        motor(LEFT_MOTOR, REVERSE ^ is_upside_down, (uint8_t)(8 * (25-ir_distance_read())));
    }
    else {
        motor(LEFT_MOTOR, FORWARD ^ is_upside_down, 0);
    }

    if (ir_val <= FORW_STOP_REV + 10) {
        obstacle = true;
    }

    if (ir_val > FORW_STOP_REV && obstacle == true) {
        if (count == 0) {
            reset_timer_counter(counter_beta);
        }
        count = counter_val;

        motor(LEFT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);

        if (count >= KEEP_TURNING) {
            obstacle = false;
        }
    }

    return;
}
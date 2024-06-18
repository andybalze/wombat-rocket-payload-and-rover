#include "dance_routine.h"

#include "stdbool.h"

#include "digital_io.h"
#include "motors.h"
#include "timer.h"

// Time delay macros
#define SPIN_TIME       5 * ONE_SECOND
#define SPIN_180_TIME   2 * ONE_SECOND
#define FWRD_L_TRN_TIME 5 * ONE_SECOND

// Rev winding pattern
#define SQUIGGLE 2
#define SQUIGGLE_SPEED_DIFF 50

// Meaningful comment here
#define FWRD_L_TRN_SPEED_DIFF 100

// Shake shake shake
#define SHAKE_TIME 1 * ONE_SECOND
#define SHAKE_NUM 4

// Dance routine private state machine states
typedef enum {
    INIT = 0,
    SPIN_CNTR_CLKW = 1,
    REV_180 = 2,
    FWRD_L_TRN = 3,
    SHAKE = 4,
    SPIN_CLKW = 5,
    DONE = 6
} dance_state_t;

// Dance routine state machine private function declarations
dance_state_t init_state(void);
dance_state_t spin_cntr_clkw(bool is_upside_down);
dance_state_t rev_180(bool is_upside_down);
dance_state_t fwrd_l_trn(bool is_upside_down);
dance_state_t shake(bool is_upside_down);
dance_state_t spin_clkw(bool is_upside_down);


////////////////////////////////////////////////////////////////////////////////////////////
////////// Public function definitions /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void dance_routine(bool is_upside_down) {
    dance_state_t dance_state = INIT;
    bool dont_exit = true;

    while(dont_exit == true) {

        // Dance state machine
        switch(dance_state) {
            case INIT: {
                dance_state = init_state();
                break;
            }

            case SPIN_CNTR_CLKW: {
                dance_state = spin_cntr_clkw();
                break;
            }

            case REV_180: {
                dance_state = rev_180();
                break;
            }

            case FWRD_L_TRN: {
                dance_state = fwrd_l_trn();
                break;
            }

            case SHAKE: {
                dance_state = shake();
                break;
            }

            case SPIN_CLKW: {
                dance_state =  spin_clkw();
                break;
            }

            case DONE: {
                dont_exit = false;
                break;
            }

            default: {
                dance_state = DONE;
                break;
            }
        }
    }
}

////////// Public function definitions /////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
////////// Dance routine state machine private function definitions ////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
dance_state_t init_state(void) {
    dance_state_t next_state;

    LED_set(YELLOW, OFF);

    // Exit condition: unconditional
    reset_timer_counter(counter_beta);
    next_state = SPIN_CNTR_CLKW;
    return next_state;
}


dance_state_t spin_cntr_clkw(bool is_upside_down) {
    dance_state_t next_state;

    motor(LEFT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);
    motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);

    // Exit condition
    if (get_timer_counter(counter_beta) < SPIN_TIME) {
        next_state = SPIN_CNTR_CLKW;
    }
    else {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        reset_timer_counter(counter_beta);
        next_state = REV_180;
    }
    return next_state;
}


dance_state_t rev_180(bool is_upside_down) {
    dance_state_t next_state;
    static uint16_t loop_cnt = 0;
    uint32_t timer_current;
    
    timer_current = get_timer_counter(counter_beta);

    if (timer_current < SQUIGGLE || timer_current > SQUIGGLE*3) {
        motor(LEFT_MOTOR, (REVERSE ^ is_upside_down) - SQUIGGLE_SPEED_DIFF, SPEED_MAX);
        motor(RIGHT_MOTOR, (REVERSE ^ is_upside_down), SPEED_MAX);
    }
    else {
        motor(LEFT_MOTOR, (REVERSE ^ is_upside_down), SPEED_MAX);
        motor(RIGHT_MOTOR, (REVERSE ^ is_upside_down) - SQUIGGLE_SPEED_DIFF, SPEED_MAX);
    }

    // exit condition
    if (get_timer_counter(counter_beta) < SQUIGGLE*4) {
        next_state = REV_180;
    }
    else {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        reset_timer_counter(counter_beta);
        next_state = FWRD_L_TRN;
    }
    return next_state;
}


dance_state_t fwrd_l_trn(bool is_upside_down) {
    dance_state_t next_state;

    motor(LEFT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX - FWRD_L_TRN_SPEED_DIFF);
    motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);

    // Exit condition
    if (get_timer_counter(counter_beta) < FWRD_L_TRN_TIME) {
        next_state = FWRD_L_TRN;
    }
    else {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        reset_timer_counter(counter_beta);
        next_state = SHAKE;
    }
    return next_state;
}


dance_state_t shake(bool is_upside_down) {
    dance_state_t next_state;
    uint32_t counter_val;

    counter_val = get_timer_counter(counter_beta);

    switch(counter_val) {
        case 0 ... SHAKE_TIME-1: {
            motor(LEFT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
            motor(RIGHT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);
            break;
        }

        case SHAKE_TIME ... SHAKE_TIME*3-1: {
            motor(LEFT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);
            motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
            break;
        }

        case SHAKE_TIME*3 ... SHAKE_TIME*5-1: {
            motor(LEFT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
            motor(RIGHT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);
            break;
        }

        case SHAKE_TIME*5 ... SHAKE_TIME*6-1 {
            motor(LEFT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);
            motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
            break;
        }

        default: {
            motor(LEFT_MOTOR, FORWARD, 0);
            motor(RIGHT_MOTOR, FORWARD, 0);
            break;
        }
    }

    // Exit condition
    if (counter_val < SHAKE_TIME*6) {
        next_state = SHAKE;
    }
    else {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        reset_timer_counter(counter_beta);
        next_state = SPIN_CLKW;
    }
    next_state = SPIN_CLKW;
    return next_state;
}


dance_state_t spin_clkw(bool is_upside_down) {
    dance_state_t next_state;

    motor(LEFT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
    motor(RIGHT_MOTOR, REVERSE ^ is_upside_down, SPEED_MAX);

    // Exit condition
    if (get_timer_counter(counter_beta) < SPIN_TIME) {
        next_state = SPIN_CLKW;
    }
    else {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        reset_timer_counter(counter_beta);
        next_state = DONE;
    }
    return next_state;
}

////////// Dance routine state machine private function definitions ////////////////////////
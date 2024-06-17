#include "dance_routine.h"

#include "stdbool.h"

#include "digital_io.h"
#include "motors.h"
#include "timer.h"

// Time delay macros
#define SPIN_TIME       5 * ONE_SECOND
#define SPIN_180_TIME   2 * ONE_SECOND
#define FWRD_L_TRN_TIME 5 * ONE_SECOND

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
dance_state_t spin_cntr_clkw(void);
dance_state_t rev_180(void);
dance_state_t fwrd_l_trn(void);
dance_state_t shake(void);
dance_state_t spin_clkw(void);


////////////////////////////////////////////////////////////////////////////////////////////
////////// Public function definitions /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

void dance_routine(void) {
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


dance_state_t spin_cntr_clkw(void) {
    dance_state_t next_state;

    motor(LEFT_MOTOR, REVERSE, 249);
    motor(RIGHT_MOTOR, FORWARD, 249);

    // Exit condition
    next_state = get_timer_counter(counter_beta) < SPIN_TIME ? SPIN_CNTR_CLKW : REV_180;
    return next_state;
}


dance_state_t rev_180(void) {
    dance_state_t next_state;

    

    next_state = FWRD_L_TRN;
    return next_state;
}


dance_state_t fwrd_l_trn(void) {
    dance_state_t next_state;

    

    next_state = SHAKE;
    return next_state;
}


dance_state_t shake(void) {
    dance_state_t next_state;

    

    next_state = SPIN_CLKW;
    return next_state;
}


dance_state_t spin_clkw(void) {
    dance_state_t next_state;

    

    next_state = DONE;
    return next_state;
}

////////// Dance routine state machine private function definitions ////////////////////////
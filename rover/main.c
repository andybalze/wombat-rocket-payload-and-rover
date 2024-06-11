////////////////////////////////////////////////////////////////////////////////
//
// Rover Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>


#include "config.h"
#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "timer.h"
#include "accelerometer.h"
#include "ir.h"
#include "avoid_obstacles.h"



# define MAX_DATA_CUBE_INV 3


/////////////////// Private Typedefs ///////////////////////////////////////////

typedef enum {
    RESET,
    MANUAL_LOAD_MODE,
    FLIGHT_MODE
} rover_mode_t;

typedef enum {
    WAIT_FOR_LAUNCH,
    WAIT_FOR_LANDING,
    EXIT_CANISTER,
    DRIVE_FORWARD,
    DISPENSE_DATA_CUBE,
    SIGNAL_ONBOARD_DATA_CUBE,
    DEAD_LOOP
} flight_state_t;



/////////////////// Private Global Variables ///////////////////////////////////

// Set true to end rover operation and enter a dead loop.
static volatile bool end_operation = false;

// Set after rover exits canister. True: rover is upside down, False: rover is right-side up
// Joey sets before exiting canister. Wombat sets after exiting canister.
static volatile bool is_upside_down;

// Keeps track of number of data cubes dispensed by the rover.
static volatile uint8_t cubes_dispensed = 0;



/////////////////// Private Function Prototypes ///////////////////////////////

// The code that's run each time around the state machine loop, depending on
// the state machine's current state. These functions handle any behavior of
// their states, as well as transitions to any other states.

// The rover_mode state machine changes between load/unload mode and flight mode
// based off SW2 on the rover controller board.
rover_mode_t rover_mode_state_reset(void);
rover_mode_t rover_mode_state_manual_load(void);
rover_mode_t rover_mode_state_flight(bool reset_flight_state);

// The flight_state state machine is inside the rover_mode state machine's flight_mode state
flight_state_t flight_state_wait_for_launch(void);
flight_state_t flight_state_wait_for_landing(void);
flight_state_t flight_state_exit_canister(void);
flight_state_t flight_state_drive_forward(void);
flight_state_t flight_state_dispense_data_cube(void);
flight_state_t flight_state_signal_onboard_data_cube(void);



/////////////////// Public Function Bodies ////////////////////////////////////

// main
int main() {

    rover_mode_t rover_mode = RESET;    // Current state of rover mode state machine
    bool reset_flight_state = true;     // Set true to set flight state to WAIT_FOR_LAUNCH

    // Initialize functions
    digital_io_initialize();
    ir_initialize();
    uart_initialize();
    adc_initialize();
    motors_initialize();                // PWM must be initialized seperately

    timer_counter_initialize();


    while(1) { // Begin main loop
        if (end_operation == true) {
            break;
        }

        switch (rover_mode) {   // Rover mode state machine (reset, manual load/unload, flight)
            case RESET: {
                rover_mode = rover_mode_state_reset();
                reset_flight_state = true;
                break;
            }

            case MANUAL_LOAD_MODE: {
                rover_mode = rover_mode_state_manual_load();
                break;
            }

            case FLIGHT_MODE: {
                rover_mode = rover_mode_state_flight(reset_flight_state);
                reset_flight_state = false;
                break;
            }

            default: {
                LED_set(RED, ON);
                LED_set(GREEN, OFF);
                uart_transmit_formatted_message("ERROR 1650: rover mode switch read error\r\n");
                UART_WAIT_UNTIL_DONE();
                break;
            }
        } // end rover mode state machine
    } // end main loop


    // Turn off motors
    motor(LEFT_MOTOR, FORWARD, 0);
    motor(RIGHT_MOTOR, FORWARD, 0);
    motor(DISPENSER_MOTOR, FORWARD, 0);

    // Disable interrupts
    cli();

    uart_transmit_formatted_message("End rover operation\r\n");
    UART_WAIT_UNTIL_DONE();
    while(1);

}   // end main()



// reset state
rover_mode_t rover_mode_state_reset(void) {
    rover_mode_t rover_mode_next = RESET;       // return value

    motor(DISPENSER_MOTOR, FORWARD, OFF);
    motor(LEFT_MOTOR, FORWARD, OFF);
    motor(RIGHT_MOTOR, FORWARD, OFF);
    LED_set(YELLOW, OFF);

    // exit condition: unconditional
    if (SW_read(ROVER_MODE_SW) == 1) {  // change state to manual load mode if switch is turned to manual load
        uart_transmit_formatted_message("MANUAL_LOAD_MODE\r\n");
        UART_WAIT_UNTIL_DONE();
        rover_mode_next = MANUAL_LOAD_MODE;
    }
    else {                              // change state to flight if switch is turned to flight mode
        LED_set(GREEN, ON);
        uart_transmit_formatted_message("FLIGHT_MODE\r\n");
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("WAIT_FOR_LAUNCH\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_launch_is_a_go();
        reset_no_motion();
        reset_timer_counter(counter_alpha);
        rover_mode_next = FLIGHT_MODE;
    }

    return rover_mode_next;
}   // end rover_mode_state_reset()



// manual load mode
rover_mode_t rover_mode_state_manual_load(void) {
    rover_mode_t rover_mode_next = MANUAL_LOAD_MODE;        // return value

    LED_set(YELLOW, ON);

    if (SW_read(LOAD_BTN) == 1) {
        motor(DISPENSER_MOTOR, REVERSE, SPEED_MAX);
    }
    else if (SW_read(UNLOAD_BTN) == 1) {
        motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);
    }
    else {
        motor(DISPENSER_MOTOR, FORWARD, 0);
    }

    //exit condition: switch turned to flight mode
    if (SW_read(ROVER_MODE_SW) == 0) {
        rover_mode_next = RESET;
    }
}   // end rover_mode_state_manual_load()



// flight mode state machine
rover_mode_t rover_mode_state_flight(bool reset_flight_state) {
    rover_mode_t rover_mode_next = FLIGHT_MODE;      // return value
    static flight_state_t flight_state;

    if (reset_flight_state == true) {
        flight_state = WAIT_FOR_LAUNCH;
    }

    switch (flight_state) { // flight-state state machine (7 states)
        case WAIT_FOR_LAUNCH:
            flight_state = flight_state_wait_for_launch();
            break;

        case WAIT_FOR_LANDING:
            flight_state = flight_state_wait_for_landing();
            break;

        case EXIT_CANISTER:
            flight_state = flight_state_exit_canister();
            break;

        case DRIVE_FORWARD:
            flight_state = flight_state_drive_forward();
            break;

        case DISPENSE_DATA_CUBE:
            flight_state = flight_state_dispense_data_cube();
            break;

        case SIGNAL_ONBOARD_DATA_CUBE:
            flight_state = flight_state_signal_onboard_data_cube();
            break;

        case DEAD_LOOP:
            end_operation = true;
            break;

        default:
            LED_set(RED, ON);
            LED_set(GREEN, OFF);
            uart_transmit_formatted_message("ERROR 1674: flight mode state machine error\r\n");
            UART_WAIT_UNTIL_DONE();
            end_operation = true;
            break;

    }   // end flight-state state machine

    // exit condition: switch turned to manual load mode
    if (SW_read(ROVER_MODE_SW) == 1) {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        motor(DISPENSER_MOTOR, FORWARD, 0);
        launch_check_disable();
        rover_mode_next = RESET;
    }

    return rover_mode_next;
}   // end rover_mode_state_flight()



// wait for launch
flight_state_t flight_state_wait_for_launch(void) {
    flight_state_t flight_state_next = WAIT_FOR_LAUNCH;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= WAIT_FOR_LAUNCH_LED_OFF_TIME) {
        LED_set(YELLOW, OFF);
        launch_check_enable();
    }

    // exit condition: rocket launch detected
    if (get_launch_is_a_go() == true) {
        launch_check_disable();
        LED_set(RED, ON);
        LED_set(GREEN, OFF);
        uart_transmit_formatted_message("WAIT_FOR_LANDING\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = WAIT_FOR_LANDING;
    }

    return flight_state_next;
}   // end flight_state_wait_for_launch()



// wait for landing
flight_state_t flight_state_wait_for_landing(void) {
    flight_state_t flight_state_next = WAIT_FOR_LANDING;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= WAIT_FOR_LANDING_LED_OFF_TIME) {
        LED_set(YELLOW, OFF);
    }

    // exit condition: time delay elapsed and rover not in motion
    if (current_time >= WAIT_FOR_LANDING_TIME) {
        no_motion_check_enable();
        if (get_no_motion() == true) {
            no_motion_check_disable();
            LED_set(YELLOW, OFF);
            #ifdef JOEY_EXIT_METHOD
                is_upside_down = !is_up();
            #endif
            PWM_enable();
            uart_transmit_formatted_message("EXIT_CANISTER\r\n");
            UART_WAIT_UNTIL_DONE();
            reset_timer_counter(counter_alpha);
            flight_state_next = EXIT_CANISTER;
        }
    }

    return flight_state_next;
}   // end flight_state_wait_for_landing()



// exit canister
flight_state_t flight_state_exit_canister(void) {
    flight_state_t flight_state_next = EXIT_CANISTER;     // return value
    uint32_t current_time;

    #ifdef JOEY_EXIT_METHOD
        motor(LEFT_MOTOR, FORWARD ^ is_upside_down, EXIT_SPEED);
        motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, EXIT_SPEED);
    #endif

    #ifdef WOMBAT_EXIT_METHOD
        motor(LEFT_MOTOR, FORWARD, EXIT_SPEED);
        motor(RIGHT_MOTOR, FORWARD, EXIT_SPEED);
    #endif

    current_time = get_timer_counter(counter_alpha);

    // exit condition: time delay elapsed
    if (current_time >= EXIT_TIME) {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        ir_power(ON);
        #ifdef WOMBAT_EXIT_METHOD
            is_upside_down = !is_up();
        #endif
        uart_transmit_formatted_message("DRIVE_FORWARD\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = DRIVE_FORWARD;
    }

    return flight_state_next;
}   // end flight_state_exit_canister()



// drive forward
flight_state_t flight_state_drive_forward(void) {
    flight_state_t flight_state_next = DRIVE_FORWARD;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= DRIVE_FORWARD_DELAY) {
        avoid(is_upside_down);
    }

    // exit condition: time delay elapsed
    if (current_time >= DRIVE_TIME) {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        ir_power(OFF);
        uart_transmit_formatted_message("DISPENSE_DATA_CUBE %d\r\n", cubes_dispensed+1);
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = DISPENSE_DATA_CUBE;
    }

    return flight_state_next;
} // end flight_state_drive_forward()



// dispense data cube
flight_state_t flight_state_dispense_data_cube(void) {
    flight_state_t flight_state_next = DISPENSE_DATA_CUBE;     // return value
    uint32_t current_time;

    motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);
    current_time = get_timer_counter(counter_alpha);

    // exit condition: time delay elapsed
    if ((current_time >= DISPENSE_TIME) && (cubes_dispensed < MAX_DATA_CUBE_INV-1)) {   // change state to drive if not all cubes dispensed
        motor(DISPENSER_MOTOR, FORWARD, 0);
        ir_power(ON);
        cubes_dispensed++;
        uart_transmit_formatted_message("DRIVE_FORWARD\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = DRIVE_FORWARD;
    }
    else if (current_time >= DISPENSE_TIME) {   // change state to signal data cube if all data cubes dispensed (drive on state transition)
        motor(DISPENSER_MOTOR, FORWARD, 0);
        uart_transmit_formatted_message("SIGNAL_ONBOARD_DATA_CUBE\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = SIGNAL_ONBOARD_DATA_CUBE;
    }

    return flight_state_next;
}   // end flight_state_drive_forward



// signal onboard data cube
flight_state_t flight_state_signal_onboard_data_cube(void) {
    flight_state_t flight_state_next = SIGNAL_ONBOARD_DATA_CUBE;     // return value
    uint32_t current_time;

    avoid(is_upside_down);

    current_time = get_timer_counter(counter_alpha);

    // exit condition: time delay elapsed
    if (current_time >= DRIVE_TIME) {
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        signal_data_cube(ON);
        LED_set(GREEN, ON);
        LED_set(RED, OFF);
        uart_transmit_formatted_message("DEAD_LOOP\r\n");
        UART_WAIT_UNTIL_DONE();
        flight_state_next = DEAD_LOOP;
    }

    return flight_state_next;
}   // end flight_state_signal_onboard_data_cube()



// dead loop
// no state function for dead loop state
// instead, main loop is exitted and enters a dead loop at the end of main()

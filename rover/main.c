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



/////////////////// Private Function Prototypes ///////////////////////////////

// The code that's run each time around the state machine loop, depending on
// the state machine's current state. These functions handle any behavior of
// their states, as well as transitions to any other states.

// The rover_mode state machine changes between load/unload mode and flight mode
// based off SW2 on the rover controller board.
rover_mode_t rover_mode_state_reset(void);
rover_mode_t rover_mode_state_manual_load(void);
rover_mode_t rover_mode_state_flight(void);

// The flight_state state machine is inside the rover_mode state machine's flight_mode state
flight_state_t flight_state_wait_for_launch(void);
flight_state_t flight_state_wait_for_landing(void);
flight_state_t flight_state_exit_canister(void);
flight_state_t flight_state_drive_forward(void);
flight_state_t flight_state_dispense_data_cube(void);
flight_state_t flight_state_signal_onboard_data_cube(void);
flight_state_t flight_state_dead_loop(void);



/////////////////// Public Function Bodies ////////////////////////////////////

int main() {

    rover_mode_t   rover_mode = RESET;

    bool end_operation = false;
    bool is_upside_down;

    // uint32_t current_time;
    uint8_t cubes_dispensed = 0;

    // Initialize functions
    digital_io_initialize();
    ir_initialize();
    uart_initialize();
    adc_initialize();
    motors_initialize();            // PWM must be initialized seperately

    timer_counter_initialize();


    // Begin main loop
    while(1) {
        if (end_operation == true) {
            break;
        }

        switch (rover_mode) {
            case RESET: {
                rover_mode = rover_mode_state_reset();
                flight_state = WAIT_FOR_LAUNCH;
                break;
            }

            case MANUAL_LOAD_MODE: {
                rover_mode = rover_mode_state_manual_load();
                break;
            }

            case FLIGHT_MODE: {
                rover_mode = rover_mode_state_flight();
                break;
            }

            default: {
                LED_set(RED, ON);
                LED_set(GREEN, OFF);
                uart_transmit_formatted_message("ERROR 1650: rover mode switch read error\r\n");
                UART_WAIT_UNTIL_DONE();
                break;
            }
        }
    }


    uart_transmit_formatted_message("End rover operation\r\n");
    UART_WAIT_UNTIL_DONE();
    while(1);

}   // end main()



rover_mode_t rover_mode_state_reset(void) {
    rover_mode_t rover_mode_next = RESET;

    motor(DISPENSER_MOTOR, FORWARD, OFF);                               //             turn off dispenser motor
    motor(LEFT_MOTOR, FORWARD, OFF);                                    //             turn off drive motors
    motor(RIGHT_MOTOR, FORWARD, OFF);
    LED_set(YELLOW, OFF);                                               //             LED off

    if (SW_read(ROVER_MODE_SW) == 1) {                                  //             exit condition if (rover mode switch is manual load)
        uart_transmit_formatted_message("MANUAL_LOAD_MODE\r\n");
        UART_WAIT_UNTIL_DONE();
        rover_mode_next = MANUAL_LOAD_MODE;                                  //                 change state to MANUAL_LOAD_MODE
    }                                                                   //             end if
    else {                                                              //             else if (rover mode switch is flight)
        LED_set(GREEN, ON);                                             //                     LED solid green
        uart_transmit_formatted_message("FLIGHT_MODE\r\n");
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("WAIT_FOR_LAUNCH\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_launch_is_a_go();
        reset_no_motion();
        reset_timer_counter(counter_alpha);
        rover_mode_next = FLIGHT_MODE;                                       //                 change state to FLIGHT_MODE
    }                                                                   //             end exit condition

    return rover_mode_next;
}   // end rover_mode_state_reset()



rover_mode_t rover_mode_state_manual_load(void) {
    rover_mode_t rover_mode_next = MANUAL_LOAD_MODE;

    LED_set(YELLOW, ON);                                                //             LED solid yellow

    if (SW_read(LOAD_BTN) == 1) {                                       //             if (load button pressed)
        motor(DISPENSER_MOTOR, REVERSE, SPEED_MAX);                     //                 load dispenser
    }                                                                   //             end if
    else if (SW_read(UNLOAD_BTN) == 1) {                                //             else if (unload button pressed)
        motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);                     //                 unload dispenser
    }                                                                   //             end if
    else {                                                              //             else if (neither button pressed)
        motor(DISPENSER_MOTOR, FORWARD, 0);                             //                 turn off dispenser motor
    }                                                                   //             end if

    if (SW_read(ROVER_MODE_SW) == 0) {                                  //             exit condition if (rover mode switch is flight)
        rover_mode_next = RESET;                                             //                 change state to RESET
    }                                                                   //             end exit condition
}   // end rover_mode_state_manual_load()



rover_mode_t rover_mode_state_flight(void) {
    rover_mode_t rover_mode_next = FLIGHT_MODE;      // return value
    flight_state_t flight_state;

    switch (flight_state) {
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

    }   // end flight state switch

    if (SW_read(ROVER_MODE_SW) == 1) {                                  //             exit condition if (rover mode switch is manual load)
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        motor(DISPENSER_MOTOR, FORWARD, 0);
        rover_mode_next = RESET;                                             //                 change state to RESET
    }                                                                   //             end exit condition

    return rover_mode_next;
}   // end rover_mode_state_flight()



flight_state_t flight_state_wait_for_launch(void) {
    flight_state_t flight_state_next = WAIT_FOR_LAUNCH;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= WAIT_FOR_LAUNCH_LED_OFF_TIME) {
        LED_set(YELLOW, OFF);
        launch_check_enable();
    }

    if (get_launch_is_a_go() == true) {                         //                     exit condition if (rocket launched)
        launch_check_disable();
        LED_set(RED, ON);
        LED_set(GREEN, OFF);
        uart_transmit_formatted_message("WAIT_FOR_LANDING\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = WAIT_FOR_LANDING;                        //                         change state to WAIT_FOR_LANDING
    }                                                           //                     end exit condition

    return flight_state_next;
}   // end flight_state_wait_for_launch()



flight_state_t flight_state_wait_for_landing(void) {
    flight_state_t flight_state_next = WAIT_FOR_LANDING;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= WAIT_FOR_LANDING_LED_OFF_TIME) {
        LED_set(YELLOW, OFF);
    }

    if (current_time >= WAIT_FOR_LANDING_TIME) {//                     exit condition
        no_motion_check_enable();
        if (get_no_motion() == true) {
            no_motion_check_disable();
            LED_set(YELLOW, OFF);
            is_upside_down = !is_up();          // Joey TEST // //                         determine which way up  // Joey TEST // this line goes after EXIT_CANISTER state for the Wombat
            PWM_enable();
            uart_transmit_formatted_message("EXIT_CANISTER\r\n");
            UART_WAIT_UNTIL_DONE();
            reset_timer_counter(counter_alpha);                 //                         reset timer counter
            flight_state_next = EXIT_CANISTER;                       //                         change state to EXIT_CANISTER
        }
    }                                                           //                     end exit condition

    return flight_state_next;
}   // end flight_state_wait_for_landing()



flight_state_t flight_state_exit_canister(void) {
    flight_state_t flight_state_next = EXIT_CANISTER;     // return value
    uint32_t current_time;

    motor(LEFT_MOTOR, FORWARD ^ is_upside_down, EXIT_SPEED);    //                     turn on drive motors
    motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, EXIT_SPEED);
    current_time = get_timer_counter(counter_alpha);

    if (current_time >= EXIT_TIME) {        //                     exit condition if (time delay reached)
        motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
        motor(RIGHT_MOTOR, FORWARD, 0);
        ir_power(ON);
        uart_transmit_formatted_message("DRIVE_FORWARD\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);                     //                         reset timer counter
        flight_state_next = DRIVE_FORWARD;                           //                         change state to DRIVE_FORWARD
    }                                                           //                     end exit condition

    return flight_state_next;
}   // end flight_state_exit_canister()



flight_state_t flight_state_drive_forward(void) {
    flight_state_t flight_state_next = DRIVE_FORWARD;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= DRIVE_FORWARD_DELAY) {
        avoid(is_upside_down);
    }

    if (current_time >= DRIVE_TIME) {       //                     exit condition if (time delay reached)
        motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
        motor(RIGHT_MOTOR, FORWARD, 0);
        ir_power(OFF);
        uart_transmit_formatted_message("DISPENSE_DATA_CUBE %d\r\n", cubes_dispensed+1);
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);                     //                         reset timer counter
        flight_state_next = DISPENSE_DATA_CUBE;                      //                         change state to DISPENSE_DATA_CUBE
    }                                                           //                     end exit condition

    return flight_state_next;
} // end flight_state_drive_forward()



flight_state_t flight_state_dispense_data_cube(void) {
    flight_state_t flight_state_next = DISPENSE_DATA_CUBE;     // return value
    uint32_t current_time;

    motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);                 //                     turn on dispenser motor
    current_time = get_timer_counter(counter_alpha);

    if ((current_time >= DISPENSE_TIME) && (cubes_dispensed < MAX_DATA_CUBE_INV-1)) {//                     exit condition if (time delay reached)
        motor(DISPENSER_MOTOR, FORWARD, 0);                     //                         turn off dispenser motor
        ir_power(ON);
        cubes_dispensed++;
        uart_transmit_formatted_message("DRIVE_FORWARD\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);                     //                         reset timer counter
        flight_state_next = DRIVE_FORWARD;                           //                         change state to DRIVE_FORWARD
    }
    else if (current_time >= DISPENSE_TIME) {
        motor(DISPENSER_MOTOR, FORWARD, 0);                     //                         turn off dispenser motor
        motor(LEFT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
        motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
        uart_transmit_formatted_message("SIGNAL_ONBOARD_DATA_CUBE\r\n");
        UART_WAIT_UNTIL_DONE();
        reset_timer_counter(counter_alpha);
        flight_state_next = SIGNAL_ONBOARD_DATA_CUBE;                //                         change state to SIGNAL_ONBOARD_DATA_CUBE
    }                                                           //                     end exit condition

    return flight_state_next;
}   // end flight_state_drive_forward



flight_state_t flight_state_signal_onboard_data_cube(void) {
    flight_state_t flight_state_next = DISPENSE_DATA_CUBE;     // return value
    uint32_t current_time;

    current_time = get_timer_counter(counter_alpha);

    if (current_time >= DRIVE_TIME) {                           //                     exit condition
        motor(LEFT_MOTOR, FORWARD, 0);
        motor(RIGHT_MOTOR, FORWARD, 0);
        signal_data_cube(ON);                                   //                         signal onboard data cube
        LED_set(GREEN, ON);                                     //                     LED solid green
        LED_set(RED, OFF);
        uart_transmit_formatted_message("DEAD_LOOP\r\n");
        UART_WAIT_UNTIL_DONE();
        flight_state_next = DEAD_LOOP;                               //                         change state to DEAD_LOOP
    }                                                           //                     end exit condition

    return flight_state_next;
}   // end flight_state_signal_onboard_data_cube()



flight_state_t flight_state_dead_loop(void) {

}   // end flight_state_dead_loop()

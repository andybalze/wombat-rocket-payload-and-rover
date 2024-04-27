////////////////////////////////////////////////////////////////////////////////
//
// Rover Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

// #include <math.h>   // DEBUG //


#include "config.h"
#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "timer.h"
#include "accelerometer.h"



enum rover_mode_enum {
    RESET,
    MANUAL_LOAD_MODE,
    FLIGHT_MODE
};
typedef enum rover_mode_enum rover_mode_t;

enum flight_state_enum {
    WAIT_FOR_LAUNCH,
    WAIT_FOR_LANDING,
    EXIT_CANISTER,
    DRIVE_FORWARD,
    DISPENSE_DATA_CUBES,
    SIGNAL_ONBOARD_DATA_CUBE,
    DEAD_LOOP
};
typedef enum flight_state_enum flight_state_t;



int main() {

    rover_mode_t   rover_mode = RESET;
    flight_state_t flight_state;

    bool end_operation = false;
    bool is_upside_down;

    digital_io_initialize();                                                        // initialize functions
    uart_initialize();
    adc_initialize();
    motors_initialize();            // PWM must be initialized seperately

    timer_counter_initialize();

    ////////// DEBUG //////////
    // while (!false) {
    //     uart_transmit_formatted_message("x: %d    y: %d    z: %d    agg_mag: %d    sqrt(agg_mag): %d\r\n", accelerometer_read(X_AXIS), accelerometer_read(Y_AXIS), accelerometer_read(Z_AXIS), acceleration_agg_mag(), (uint32_t)sqrt(acceleration_agg_mag()));
    //     UART_WAIT_UNTIL_DONE();
    // }
    ////////// DEBUG //////////

    while(1) {                                                                      // begin main loop
        if (end_operation == true) {
            break;
        }

        switch (rover_mode) {                                                       //     switch (rover mode)
            case RESET: {                                                           //         case (RESET)
                motor(DISPENSER_MOTOR, FORWARD, OFF);                               //             turn off dispenser motor
                motor(LEFT_MOTOR, FORWARD, OFF);                                    //             turn off drive motors
                motor(RIGHT_MOTOR, FORWARD, OFF);
                LED_set(YELLOW, OFF);                                               //             LED off

                if (SW_read(ROVER_MODE_SW) == 1) {                                  //             exit condition if (rover mode switch is manual load)
                    uart_transmit_formatted_message("MANUAL_LOAD_MODE\r\n");
                    UART_WAIT_UNTIL_DONE();
                    rover_mode = MANUAL_LOAD_MODE;                                  //                 change state to MANUAL_LOAD_MODE
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
                    flight_state = WAIT_FOR_LAUNCH;                                 //                 change flight mode state to WAIT_FOR_LAUNCH
                    rover_mode = FLIGHT_MODE;                                       //                 change state to FLIGHT_MODE
                }                                                                   //             end exit condition
                break;
            }                                                                       //         end case

            case MANUAL_LOAD_MODE: {                                                //         case (MANUAL_LOAD_MODE)
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
                    rover_mode = RESET;                                             //                 change state to RESET
                }                                                                   //             end exit condition
                break;
            }                                                                       //         end case

            case FLIGHT_MODE: {                                                     //         case (FLIGHT_MODE)
                switch (flight_state) {                                             //             switch (flight state)
                    case WAIT_FOR_LAUNCH: {                                         //                 case (WAIT_FOR_LAUNCH)
                        if (get_timer_counter(counter_alpha) >= WAIT_FOR_LAUNCH_LED_OFF_TIME) {
                            LED_set(YELLOW, OFF);
                            launch_check_enable();
                        }

                        if (get_launch_is_a_go() == true) {                               //                     exit condition if (rocket launched)
                            launch_check_disable();
                            LED_set(RED, ON);
                            LED_set(GREEN, OFF);
                            uart_transmit_formatted_message("WAIT_FOR_LANDING\r\n");
                            UART_WAIT_UNTIL_DONE();
                            reset_timer_counter(counter_alpha);
                            flight_state = WAIT_FOR_LANDING;                        //                         change state to WAIT_FOR_LANDING
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case WAIT_FOR_LANDING: {                                        //                 case (WAIT_FOR_LANDING)
                        if (get_timer_counter(counter_alpha) >= WAIT_FOR_LANDING_LED_OFF_TIME) {
                            LED_set(YELLOW, OFF);
                        }

                        if (get_timer_counter(counter_alpha) >= WAIT_FOR_LANDING_TIME) {  //                     exit condition
                            no_motion_check_enable();   // TEST //          // Has to be in here b.c. counter can't work during motion check
                            if (get_no_motion() == true) {  // TEST //
                                no_motion_check_disable();  // TEST //
                                LED_set(YELLOW, OFF);
                                is_upside_down = !is_up();                                  //                         determine which way up  // Joey TEST // this line goes after EXIT_CANISTER state for the Wombat
                                PWM_enable();
                                uart_transmit_formatted_message("EXIT_CANISTER\r\n");
                                UART_WAIT_UNTIL_DONE();
                                reset_timer_counter(counter_alpha);                               //                         reset timer counter
                                flight_state = EXIT_CANISTER;                           //                         change state to EXIT_CANISTER
                            }
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case EXIT_CANISTER: {                                           //                 case (EXIT_CANISTER)
                        motor(LEFT_MOTOR, FORWARD ^ is_upside_down, EXIT_SPEED);                     //                     turn on drive motors
                        motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, EXIT_SPEED);

                        if (get_timer_counter(counter_alpha) >= EXIT_TIME) {              //                     exit condition if (time delay reached)
                            motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            uart_transmit_formatted_message("DRIVE_FORWARD\r\n");
                            UART_WAIT_UNTIL_DONE();
                            reset_timer_counter(counter_alpha);                               //                         reset timer counter
                            flight_state = DRIVE_FORWARD;                           //                         change state to DRIVE_FORWARD
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DRIVE_FORWARD: {                                           //                 case (DRIVE_FORWARD)
                        if (get_timer_counter(counter_alpha) >= DRIVE_FORWARD_DELAY) {
                            motor(LEFT_MOTOR, (FORWARD ^ is_upside_down), DRIVE_SPEED);//                     drive forward
                            motor(RIGHT_MOTOR, (FORWARD ^ is_upside_down), DRIVE_SPEED);
                        }

                        if (get_timer_counter(counter_alpha) >= DRIVE_TIME) {             //                     exit condition if (time delay reached)
                            motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            uart_transmit_formatted_message("DISPENSE_DATA_CUBE\r\n");
                            UART_WAIT_UNTIL_DONE();
                            reset_timer_counter(counter_alpha);                     //                         reset timer counter
                            flight_state = DISPENSE_DATA_CUBES;                     //                         change state to DISPENSE_DATA_CUBES
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DISPENSE_DATA_CUBES: {                                     //                 case (DISPENSE_DATA_CUBES)
                        motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);                 //                     turn on dispenser motor

                        if (get_timer_counter(counter_alpha) >= DISPENSE_TIME) {    //                     exit condition if (time delay reached)
                            motor(DISPENSER_MOTOR, FORWARD, 0);                     //                         turn off dispenser motor
                            motor(LEFT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
                            motor(RIGHT_MOTOR, FORWARD ^ is_upside_down, SPEED_MAX);
                            uart_transmit_formatted_message("SIGNAL_ONBOARD_DATA_CUBE\r\n");
                            UART_WAIT_UNTIL_DONE();
                            reset_timer_counter(counter_alpha);
                            flight_state = SIGNAL_ONBOARD_DATA_CUBE;                //                         change state to SIGNAL_ONBOARD_DATA_CUBE
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case SIGNAL_ONBOARD_DATA_CUBE: {                                //                 case (SIGNAL_ONBOARD_DATA_CUBE)
                        if (get_timer_counter(counter_alpha) >= SIGNAL_ONBOARD_DATA_CUBE_TIME) { //                     exit condition
                            motor(LEFT_MOTOR, FORWARD, 0);
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            signal_data_cube(ON);                                   //                         signal onboard data cube
                            LED_set(GREEN, ON);                                     //                     LED solid green
                            LED_set(RED, OFF);
                            uart_transmit_formatted_message("DEAD_LOOP\r\n");
                            UART_WAIT_UNTIL_DONE();
                            flight_state = DEAD_LOOP;                               //                         change state to DEAD_LOOP
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DEAD_LOOP: {                                               //                 case (DEAD_LOOP)
                        end_operation = true;                                          //                     dead loop
                        break;
                    }                                                               //                 end case

                    default: {                                                      //                 default case
                        LED_set(RED, ON);                                           //                     LED solid red
                        LED_set(GREEN, OFF);
                        uart_transmit_formatted_message("ERROR 1674: flight mode state machine error\r\n");
                        UART_WAIT_UNTIL_DONE();
                        end_operation = true;                                          //                     enter rover error state
                        break;
                    }                                                               //                 end case
                }                                                                   //             end switch

                if (SW_read(ROVER_MODE_SW) == 1) {                                  //             exit condition if (rover mode switch is manual load)
                    motor(LEFT_MOTOR, FORWARD, 0);
                    motor(RIGHT_MOTOR, FORWARD, 0);
                    motor(DISPENSER_MOTOR, FORWARD, 0);
                    rover_mode = RESET;                                             //                 change state to RESET
                }                                                                   //             end exit condition
                break;
            }                                                                       //         end case

            default: {                                                              //         default case
                LED_set(RED, ON);                                                   //             LED solid red
                LED_set(GREEN, OFF);                                                //             enter rover error state
                uart_transmit_formatted_message("ERROR 1650: rover mode switch read error\r\n");
                UART_WAIT_UNTIL_DONE();
                break;
            }                                                                       //         end case
        }                                                                           //     end switch
    }                                                                               // end main loop


    uart_transmit_formatted_message("End rover operation\r\n");
    UART_WAIT_UNTIL_DONE();
    while(1);

}

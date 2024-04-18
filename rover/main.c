////////////////////////////////////////////////////////////////////////////////
//
// Rover Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>              // DEBUG // Do I need this? Ask Michael
#include <stdint.h>
#include <stdbool.h>

#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "timer.h"


#define EXIT_SPEED      149
#define DRIVE_SPEED     149

// #define EXIT_TIME               10 * ONE_SECOND
// #define DRIVE_TIME              3  * ONE_MINUTE
// #define DISPENSE_TIME           1  * ONE_MINUTE     // 1 minute (actually takes about 35 seconds)
// #define LED_OFF_DELAY_LAUNCH    10 * ONE_SECOND     // used in WAIT_FOR_LAUNCH state

////////// TEST //////////
#define EXIT_TIME               10 * ONE_SECOND
#define DRIVE_TIME              10 * ONE_SECOND
#define DISPENSE_TIME           10 * ONE_SECOND
#define LED_OFF_DELAY_LAUNCH    2  * ONE_SECOND     // used in WAIT_FOR_LAUNCH state
////////// TEST //////////

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

//////////////////// Begin Pseudocode ////////////////////
/*
initialize functions
begin main loop
    switch (rover mode)
        case (RESET)
            turn off dispenser motor
            turn off drive motors
            LED off

            exit condition if (rover mode switch is manual load)
                change state to MANUAL_LOAD_MODE
            end if
            else if (rover mode switch is flight)
                LED solid green
                change flight mode state to WAIT_FOR_LAUNCH
                change state to FLIGHT_MODE
            end exit condition
        end case

        case (MANUAL_LOAD_MODE)
            LED solid yellow

            exit condition if (rover mode switch is flight)
                change state to RESET
            end exit condition

            if (load button pressed)
                load dispenser
            end if
            else if (unload button pressed)
                unload dispenser
            end if
            else if (neither button pressed)
                turn off dispenser motor
            end if
        end case

        case (FLIGHT_MODE)
            exit condition if (rover mode switch is manual load)
                change state to RESET
            end exit condition

            switch (flight state)
                case (WAIT_FOR_LAUNCH)
                    is_launched = launch check function

                    exit condition if (rocket launched)
                        LED off
                        change state to WAIT_FOR_LANDING
                    end exit condition
                end case

                case (WAIT_FOR_LANDING)
                    wait until landing code (doesn't return until landing)

                    exit condition (unconditional)
                        reset timer counter
                        change state to EXIT_CANISTER
                    end exit condition
                end case

                case (EXIT_CANISTER)
                    exit condition if (time delay reached)
                        turn off drive motors
                        determine which way up
                        reset timer counter
                        change state to DRIVE_FORWARD
                    end exit condition

                    turn on drive motors
                end case

                case (DRIVE_FORWARD)
                    exit condition if (time delay reached)
                        turn off drive motors
                        reset timer counter
                        change state to DISPENSE_DATA_CUBES
                    end exit condition

                    drive forward
                end case

                case (DISPENSE_DATA_CUBES)
                    exit condition if (time delay reached)
                        turn off dispenser motor
                        change state to SIGNAL_ONBOARD_DATA_CUBE
                    end exit condition

                    turn on dispenser motor
                end case

                case (SIGNAL_ONBOARD_DATA_CUBE)
                    exit condition (unconditional)
                        signal onboard data cube
                        LED solid green
                        change state to DEAD_LOOP
                    end exit condition
                end case

                case (DEAD_LOOP)
                    dead loop
                end case

                default case
                    LED solid red
                    enter rover error state
                end case
            end switch
        end case

        default case
            LED solid red
            enter rover error state
        end case

    end switch
end main loop
*/
//////////////////// End Pseudocode ////////////////////

int main() {

    rover_mode_t   rover_mode = RESET;
    flight_state_t flight_state;

    bool end_operation = false;
    bool is_launched;
    bool is_upside_down;

    digital_io_initialize();                                                        // initialize functions
    uart_initialize();
    adc_initialize();
    motors_initialize();
    timer_initialize();

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
                is_launched = false;

                if (SW_read(ROVER_MODE_SW) == 1) {                                  //             exit condition if (rover mode switch is manual load)
                    uart_transmit_formatted_message("MANUAL_LOAD_MODE\r\n");
                    UART_WAIT_UNTIL_DONE();
                    rover_mode = MANUAL_LOAD_MODE;                                  //                 change state to MANUAL_LOAD_MODE
                }                                                                   //             end if
                else {                                                              //             else if (rover mode switch is flight)
                    uart_transmit_formatted_message("FLIGHT_MODE\r\n");
                    UART_WAIT_UNTIL_DONE();
                    uart_transmit_formatted_message("WAIT_FOR_LAUNCH\r\n");
                    UART_WAIT_UNTIL_DONE();
                    LED_set(GREEN, ON);                                             //                     LED solid green
                    reset_timer(timer_alpha);
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
                        if (get_timer_cnt(timer_alpha) == LED_OFF_DELAY_LAUNCH) {
                            LED_set(YELLOW, OFF);
                        }

                        //                     is_launched = launch check function
                        ////////// TEST //////////
                        if (get_timer_cnt(timer_alpha) == 10 * ONE_SECOND) {
                            is_launched = true;
                        }
                        ////////// TEST //////////
                        if (is_launched == true) {                                          //                     exit condition if (rocket launched)
                            uart_transmit_formatted_message("WAIT_FOR_LANDING\r\n");
                            UART_WAIT_UNTIL_DONE();
                            // LED_set(YELLOW, OFF);                                   //                         LED off
                            LED_set(RED, ON);       // TEST // do we want this indicator?
                            LED_set(GREEN, OFF);    // TEST //
                            reset_timer(timer_alpha);
                            flight_state = WAIT_FOR_LANDING;                        //                         change state to WAIT_FOR_LANDING
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case WAIT_FOR_LANDING: {                                        //                 case (WAIT_FOR_LANDING)
                        //                     wait until landing code (doesn't return until landing)
                        ////////// TEST //////////
                        reset_timer(timer_alpha);
                        while (get_timer_cnt(timer_alpha) < 10 * ONE_SECOND);   // 10 seconds
                        ////////// TEST //////////

                        {                                                           //                     exit condition (unconditional)
                            LED_set(YELLOW, OFF);
                            uart_transmit_formatted_message("EXIT_CANISTER\r\n");
                            UART_WAIT_UNTIL_DONE();
                            reset_timer(timer_alpha);                                    //                         reset timer counter
                            flight_state = EXIT_CANISTER;                           //                         change state to EXIT_CANISTER
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case EXIT_CANISTER: {                                           //                 case (EXIT_CANISTER)
                        motor(LEFT_MOTOR, FORWARD, EXIT_SPEED);                     //                     turn on drive motors
                        motor(RIGHT_MOTOR, FORWARD, EXIT_SPEED);

                        if (get_timer_cnt(timer_alpha) == EXIT_TIME) {                   //                     exit condition if (time delay reached)
                            uart_transmit_formatted_message("DRIVE_FORWARD\r\n");
                            UART_WAIT_UNTIL_DONE();
                            motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            is_upside_down = true;         // TEST //                  //                         determine which way up
                            reset_timer(timer_alpha);                                    //                         reset timer counter
                            flight_state = DRIVE_FORWARD;                           //                         change state to DRIVE_FORWARD
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DRIVE_FORWARD: {                                           //                 case (DRIVE_FORWARD)
                        motor(LEFT_MOTOR, (FORWARD ^ is_upside_down), DRIVE_SPEED); //                     drive forward
                        motor(RIGHT_MOTOR, (FORWARD ^ is_upside_down), DRIVE_SPEED);

                        if (get_timer_cnt(timer_alpha) == DRIVE_TIME) {                  //                     exit condition if (time delay reached)
                            uart_transmit_formatted_message("DISPENSE_DATA_CUBE\r\n");
                            UART_WAIT_UNTIL_DONE();
                            motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            reset_timer(timer_alpha);                                    //                         reset timer counter
                            flight_state = DISPENSE_DATA_CUBES;                     //                         change state to DISPENSE_DATA_CUBES
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DISPENSE_DATA_CUBES: {                                     //                 case (DISPENSE_DATA_CUBES)
                        motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);                 //                     turn on dispenser motor

                        if (get_timer_cnt(timer_alpha) == DISPENSE_TIME) {               //                     exit condition if (time delay reached)
                            uart_transmit_formatted_message("SIGNAL_ONBOARD_DATA_CUBE\r\n");
                            UART_WAIT_UNTIL_DONE();
                            motor(DISPENSER_MOTOR, FORWARD, 0);                     //                         turn off dispenser motor
                            flight_state = SIGNAL_ONBOARD_DATA_CUBE;                //                         change state to SIGNAL_ONBOARD_DATA_CUBE
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case SIGNAL_ONBOARD_DATA_CUBE: {                                //                 case (SIGNAL_ONBOARD_DATA_CUBE)
                        {                                                           //                     exit condition (unconditional)
                            uart_transmit_formatted_message("DEAD_LOOP\r\n");
                            UART_WAIT_UNTIL_DONE();
                            signal_data_cube(ON);                                   //                         signal onboard data cube
                            LED_set(GREEN, ON);                                     //                     LED solid green
                            LED_set(RED, OFF);
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

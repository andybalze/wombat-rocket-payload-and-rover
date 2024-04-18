////////////////////////////////////////////////////////////////////////////////
//
// Rover Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "timer.h"


#define EXIT_SPEED      149
#define DRIVE_SPEED     149
// #define EXIT_TIME       306     // 10 seconds
// #define DRIVE_TIME      5508    // 3 minutes
// #define DISPENSE_TIME   1836    // 1 minute (actually takes about 35 seconds)

////////// TEST //////////
#define EXIT_TIME       306     // 10 seconds
#define DRIVE_TIME      306     // 10 seconds
#define DISPENSE_TIME   306     // 10 seconds
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
    char end_operation = 0;
    char is_launched = 0;
    char is_upside_down;

    digital_io_initialize();                                                        // initialize functions
    uart_initialize();
    adc_initialize();
    motors_initialize();
    timer_initialize();

    while(1) {                                                                      // begin main loop
        if (end_operation == 1) {
            break;
        }

        switch (rover_mode) {                                                       //     switch (rover mode)
            case RESET: {                                                           //         case (RESET)
                motor(DISPENSER_MOTOR, FORWARD, OFF);                               //             turn off dispenser motor
                motor(LEFT_MOTOR, FORWARD, OFF);                                    //             turn off drive motors
                motor(RIGHT_MOTOR, FORWARD, OFF);
                LED_set(YELLOW, OFF);                                               //             LED off
                is_launched = 0;

                if (SW_read(ROVER_MODE_SW) == 1) {                                  //             exit condition if (rover mode switch is manual load)
                    uart_transmit_formatted_message("MANUAL_LOAD_MODE\r\n");    // TEST //
                    UART_WAIT_UNTIL_DONE();     // TEST //
                    rover_mode = MANUAL_LOAD_MODE;                                  //                 change state to MANUAL_LOAD_MODE
                }                                                                   //             end if
                else {                                                              //             else if (rover mode switch is flight)
                    uart_transmit_formatted_message("FLIGHT_MODE\r\n");    // TEST //
                    UART_WAIT_UNTIL_DONE();     // TEST //
                    uart_transmit_formatted_message("WAIT_FOR_LAUNCH\r\n");    // TEST //
                    UART_WAIT_UNTIL_DONE();     // TEST //
                    LED_set(GREEN, ON);                                             //                     LED solid green
                    ////////// TEST //////////
                    reset_timer(timerA);
                    ////////// TEST //////////
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
                        //                     is_launched = launch check function
                        ////////// TEST //////////
                        if (get_timer_cnt(timerA) == 306) {   // 10 seconds
                            is_launched = 1;
                        }
                        ////////// TEST //////////

                        if (is_launched) {                                          //                     exit condition if (rocket launched)
                            uart_transmit_formatted_message("WAIT_FOR_LANDING\r\n");    // TEST //
                            UART_WAIT_UNTIL_DONE();     // TEST //
                            LED_set(YELLOW, OFF);                                   //                         LED off
                            flight_state = WAIT_FOR_LANDING;                        //                         change state to WAIT_FOR_LANDING
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case WAIT_FOR_LANDING: {                                        //                 case (WAIT_FOR_LANDING)
                        //                     wait until landing code (doesn't return until landing)
                        ////////// TEST //////////
                        reset_timer(timerA);
                        while (get_timer_cnt(timerA) < 306);   // 10 seconds
                        ////////// TEST //////////

                        {                                                           //                     exit condition (unconditional)
                            uart_transmit_formatted_message("EXIT_CANISTER\r\n");    // TEST //
                            UART_WAIT_UNTIL_DONE();     // TEST //
                            reset_timer(timerA);                                    //                         reset timer counter
                            flight_state = EXIT_CANISTER;                           //                         change state to EXIT_CANISTER
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case EXIT_CANISTER: {                                           //                 case (EXIT_CANISTER)
                        motor(LEFT_MOTOR, FORWARD, EXIT_SPEED);                     //                     turn on drive motors
                        motor(RIGHT_MOTOR, FORWARD, EXIT_SPEED);

                        if (get_timer_cnt(timerA) == EXIT_TIME) {                   //                     exit condition if (time delay reached)
                            uart_transmit_formatted_message("DRIVE_FORWARD\r\n");    // TEST //
                            UART_WAIT_UNTIL_DONE();     // TEST //
                            motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            is_upside_down = 1;         // TEST //                  //                         determine which way up
                            reset_timer(timerA);                                    //                         reset timer counter
                            flight_state = DRIVE_FORWARD;                           //                         change state to DRIVE_FORWARD
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DRIVE_FORWARD: {                                           //                 case (DRIVE_FORWARD)
                        motor(LEFT_MOTOR, (FORWARD ^ is_upside_down), DRIVE_SPEED); //                     drive forward
                        motor(RIGHT_MOTOR, (FORWARD ^ is_upside_down), DRIVE_SPEED);

                        if (get_timer_cnt(timerA) == DRIVE_TIME) {                  //                     exit condition if (time delay reached)
                            uart_transmit_formatted_message("DISPENSE_DATA_CUBE\r\n");    // TEST //
                            UART_WAIT_UNTIL_DONE();     // TEST //
                            motor(LEFT_MOTOR, FORWARD, 0);                          //                         turn off drive motors
                            motor(RIGHT_MOTOR, FORWARD, 0);
                            reset_timer(timerA);                                    //                         reset timer counter
                            flight_state = DISPENSE_DATA_CUBES;                     //                         change state to DISPENSE_DATA_CUBES
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DISPENSE_DATA_CUBES: {                                     //                 case (DISPENSE_DATA_CUBES)
                        motor(DISPENSER_MOTOR, FORWARD, SPEED_MAX);                 //                     turn on dispenser motor

                        if (get_timer_cnt(timerA) == DISPENSE_TIME) {               //                     exit condition if (time delay reached)
                            uart_transmit_formatted_message("SIGNAL_ONBOARD_DATA_CUBE\r\n");    // TEST //
                            UART_WAIT_UNTIL_DONE();     // TEST //
                            motor(DISPENSER_MOTOR, FORWARD, 0);                     //                         turn off dispenser motor
                            flight_state = SIGNAL_ONBOARD_DATA_CUBE;                //                         change state to SIGNAL_ONBOARD_DATA_CUBE
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case SIGNAL_ONBOARD_DATA_CUBE: {                                //                 case (SIGNAL_ONBOARD_DATA_CUBE)
                        {                                                           //                     exit condition (unconditional)
                            uart_transmit_formatted_message("DEAD_LOOP\r\n");    // TEST //
                            UART_WAIT_UNTIL_DONE();     // TEST //
                            signal_data_cube(1);                                    //                         signal onboard data cube
                            LED_set(GREEN, ON);                                     //                     LED solid green
                            LED_set(RED, OFF);
                            flight_state = DEAD_LOOP;                               //                         change state to DEAD_LOOP
                        }                                                           //                     end exit condition
                        break;
                    }                                                               //                 end case

                    case DEAD_LOOP: {                                               //                 case (DEAD_LOOP)
                        end_operation = 1;                                          //                     dead loop
                        break;
                    }                                                               //                 end case

                    default: {                                                      //                 default case
                        LED_set(RED, ON);                                           //                     LED solid red
                        LED_set(GREEN, OFF);
                        uart_transmit_formatted_message("ERROR 1674: flight mode state machine error\r\n");    // TEST //
                        UART_WAIT_UNTIL_DONE();     // TEST //
                        end_operation = 1;                                          //                     enter rover error state
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
                uart_transmit_formatted_message("ERROR 1650: rover mode switch read error\r\n");    // TEST //
                UART_WAIT_UNTIL_DONE();     // TEST //
                break;
            }                                                                       //         end case
        }                                                                           //     end switch
    }                                                                               // end main loop


    uart_transmit_formatted_message("End rover operation\r\n");     // TEST //
    UART_WAIT_UNTIL_DONE();                                         // TEST //
    while(1);

}

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
                change flight mode state to WAIT_FOR_LAUNCH
                change state to FLIGHT_MODE
            end exit condition
        end case

        case (MANUAL_LOAD_MODE)
            LED blinking yellow

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
                    LED solid green
                    is_launched = launch check function

                    exit condition if (rocket launched)
                        change state to WAIT_FOR_LANDING
                    end exit condition
                end case

                case (WAIT_FOR_LANDING)
                    LED off
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
                        change state to DRIVE_FORWARD
                        reset timer counter
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
                        change state to DEAD_LOOP
                    end exit condition
                end case

                case (DEAD_LOOP)
                    LED blinking green
                    dead loop
                end case

                default case
                    LED solid red
                    enter rover error state
                end case
            end switch
        end case

        default case
            LED blinking red
            enter rover error state
        end case

    end switch
end main loop
*/
//////////////////// End Pseudocode ////////////////////

int main() {

    rover_mode_t   rover_mode;
    flight_state_t flight_state;

    // initialize functions
    digital_io_initialize();
    uart_initialize();
    adc_initialize();
    motors_initialize();
    timer_initialize();

    while(1) { // begin main loop
        switch (rover_mode) {//     switch (rover mode)
            case RESET: {//         case (RESET)
                motor(DISPENSER_MOTOR, FORWARD, OFF);//             turn off dispenser motor
                motor(LEFT_MOTOR, FORWARD, OFF);//             turn off drive motors
                motor(RIGHT_MOTOR, FORWARD, OFF);
                LED_set(GREEN, OFF);//             LED off
                LED_set(RED, OFF);

                if () {//             exit condition if (rover mode switch is manual load)
                //                 change state to MANUAL_LOAD_MODE
                //             end if
                //             else if (rover mode switch is flight)
                //                 change flight mode state to WAIT_FOR_LAUNCH
                //                 change state to FLIGHT_MODE
                }//             end exit condition
                break;
            }//         end case

    //         case (MANUAL_LOAD_MODE)
    //             LED blinking yellow

    //             exit condition if (rover mode switch is flight)
    //                 change state to RESET
    //             end exit condition

    //             if (load button pressed)
    //                 load dispenser
    //             end if
    //             else if (unload button pressed)
    //                 unload dispenser
    //             end if
    //             else if (neither button pressed)
    //                 turn off dispenser motor
    //             end if
    //         end case

    //         case (FLIGHT_MODE)
    //             exit condition if (rover mode switch is manual load)
    //                 change state to RESET
    //             end exit condition

    //             switch (flight state)
    //                 case (WAIT_FOR_LAUNCH)
    //                     LED solid green
    //                     is_launched = launch check function

    //                     exit condition if (rocket launched)
    //                         change state to WAIT_FOR_LANDING
    //                     end exit condition
    //                 end case

    //                 case (WAIT_FOR_LANDING)
    //                     LED off
    //                     wait until landing code (doesn't return until landing)

    //                     exit condition (unconditional)
    //                         reset timer counter
    //                         change state to EXIT_CANISTER
    //                     end exit condition
    //                 end case

    //                 case (EXIT_CANISTER)
    //                     exit condition if (time delay reached)
    //                         turn off drive motors
    //                         determine which way up
    //                         change state to DRIVE_FORWARD
    //                         reset timer counter
    //                     end exit condition

    //                     turn on drive motors
    //                 end case

    //                 case (DRIVE_FORWARD)
    //                     exit condition if (time delay reached)
    //                         turn off drive motors
    //                         reset timer counter
    //                         change state to DISPENSE_DATA_CUBES
    //                     end exit condition

    //                     drive forward
    //                 end case

    //                 case (DISPENSE_DATA_CUBES)
    //                     exit condition if (time delay reached)
    //                         turn off dispenser motor
    //                         change state to SIGNAL_ONBOARD_DATA_CUBE
    //                     end exit condition

    //                     turn on dispenser motor
    //                 end case

    //                 case (SIGNAL_ONBOARD_DATA_CUBE)
    //                     exit condition (unconditional)
    //                         signal onboard data cube
    //                         change state to DEAD_LOOP
    //                     end exit condition
    //                 end case

    //                 case (DEAD_LOOP)
    //                     LED blinking green
    //                     dead loop
    //                 end case

    //                 default case
    //                     LED solid red
    //                     enter rover error state
    //                 end case
    //             end switch
    //         end case

    //         default case
    //             LED blinking red
    //             enter rover error state
    //         end case

        }//     end switch
    } // end main loop

    while(1);

}

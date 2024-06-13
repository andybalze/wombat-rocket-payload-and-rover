#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
//
// Rover Configuration
//
// Change these macros to adjust settings for various rover behavior.
//
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
////////// Rover exit method ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/*
Uncomment one of the following macros to set how the rover exits the payload
box: either determining its orientation before exiting the box or exiting the
box indescriminent of its orientation and determining orientation after exiting
the box.

The payload box has a rack on the inside that engages with the Wombat's tread,
creating a rack-and-pinion system where the Wombat's tread acts as the pinion.
This system makes it so the Wombat drives its treads the same direction
indescriminent of which orientation it is in.
*/

// Use this macro if the rover's tread engages with the payload box
#define WOMBAT_EXIT_METHOD

// Use this macro if the rover's tread relies on gravity and DOES NOT engage with the payload box
// #define JOEY_EXIT_METHOD

////////// Rover exit method ///////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
////////// Rover drive speeds //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// The speed (0-249) at which the rover drives when exiting the canister
#define EXIT_SPEED      249

// The speed (0-249) at which the rover drives after exiting the canister
#define DRIVE_SPEED     249

////////// Rover drive speeds //////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////
////////// Timing for different states of the rover's main state machine ///////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/*
Do not change the flight values for testing!
Instead, commment out the flight section and uncomment the test section
*/

////////// FLIGHT //////////
// #define WAIT_FOR_LAUNCH_LED_OFF_TIME    ((uint32_t) 10 * ONE_SECOND)         // LED turns green for this amount of time before turning off
// #define WAIT_FOR_LANDING_LED_OFF_TIME   ((uint32_t) 10 * ONE_SECOND)         // LED turns red for this amount of time before turning off

// #define WAIT_FOR_LANDING_TIME           ((uint32_t) 20 * ONE_MINUTE)         // Rover waits this amount of time after launch before attempting to exit the canister
// #define EXIT_TIME                       ((uint32_t) 5  * ONE_SECOND)         // Rover drives forward this amount of time in an attempt to exit the canister
// #define DRIVE_FORWARD_DELAY             ((uint32_t) 100)                     // Delay between exiting canister and driving forward to reduce back EMF if the rover wheels change direction
// #define DRIVE_TIME                      ((uint32_t) 10  * ONE_SECOND)        // Rover drives forward this amount of time between dispensing each cube. Joey drives just under 0.6 ft/s
// #define DISPENSE_TIME                   ((uint32_t) 12 * ONE_SECOND)         // Rover runs dispenser motor this amount of time to dispense one cube
////////// FLIGHT //////////


////////// TEST //////////
#define WAIT_FOR_LAUNCH_LED_OFF_TIME    ((uint32_t) 2  * ONE_SECOND)
#define WAIT_FOR_LANDING_LED_OFF_TIME   ((uint32_t) 2  * ONE_SECOND)

#define WAIT_FOR_LANDING_TIME           ((uint32_t) 3  * ONE_SECOND)
#define EXIT_TIME                       ((uint32_t) 2  * ONE_SECOND)
#define DRIVE_FORWARD_DELAY             ((uint32_t) 100)
#define DRIVE_TIME                      ((uint32_t) 2  * ONE_SECOND)    //84 * ONE_SECOND      // About 0.6 ft/s
#define DISPENSE_TIME                   ((uint32_t) 12 * ONE_SECOND)    // To dispense one cube
////////// TEST //////////

////////// Timing for different states of the rover's main state machine ///////////////////



////////////////////////////////////////////////////////////////////////////////////////////
////////// Launch detection settings ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

#define LAUNCH_FORCE 3                                      // Force in Gs the accelerometer watches for to detect launch

// #define LAUNCH_FORCE_CNT_THRESHOLD 58                       // Number of samples out of 64 that must be >= LAUNCH_FORCE to trigger is_launched
                                                            // 58 is 90% of 64 samples

#define LAUNCH_FORCE_CNT_THRESHOLD 2                      // TEST //

////////// Launch detection settings ///////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
////////// No movement detection settings //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// Threshold of acceleration in (1/2) m/s/s the accelerometer watches for to detect that the rover is not moving
// Threshold is plus/minus so the actual tolerance is twice as much as entered below
// No motion is 19.6 (1/2) m/s/s
#define NO_MOVEMENT_TOLERANCE 5

////////// No movement detection settings //////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////
////////// Accelerometer Callibration //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/*
Uncomment the section for the rover controller serial number you are using.
Current launch detection code is designed to be unaffected from a miscalibrated accelerometer.
*/

////////// Rover Controller SN1 //////////
// #define X_AXIS_ERROR 0
// #define Y_AXIS_ERROR 0
// #define Z_AXIS_ERROR 0
////////// Rover Controller SN1 //////////


////////// Rover Controller SN2 //////////
#define X_AXIS_ERROR 0
#define Y_AXIS_ERROR 0
#define Z_AXIS_ERROR 0
////////// Rover Controller SN2 //////////

////////// Accelerometer Callibration //////////////////////////////////////////////////////

#endif // CONFIG_H
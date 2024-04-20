#ifndef _ACCELEROMETER_H
#define _ACCELEROMETER_H

////////////////////////////////////////////////////////////////////////////////
//
// Accelerometer
//
// Provides functions that allow designs to use the on board accelerometer using
// the ATMega328p's ADC peripheral.
//
////////////////////////////////////////////////////////////////////////////////


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>


///////////////////// Macro Definitions /////////////////////////////////////////

// Don't use ONE_G macro too freely! You'll introduce big rounding errors!
// 1 G is actually 19.6 (1/2)*m/(s^2). I rounded down so is_launched is slightly more sensitive.
// #define ONE_G                   20          // Use with caution
#define ONE_G_SQUARED           385         // Also use with caution

#define LAUNCH_FORCE            3           // Over 3 G at launch. This is the thing you change if you want to adjust rocket launch sensing sensitivity
#define LAUNCH_FORCE_SQUARED    LAUNCH_FORCE * LAUNCH_FORCE * ONE_G_SQUARED
//#define LAUNCH_FORCE_SQUARED    420

#define LAUNCH_FORCE_CNT_THRESHOLD 58       // 58 is 90% of 64 samples
//#define LAUNCH_FORCE_CNT_THRESHOLD 2       // 58 is 90% of 64 samples




///////////////////// Type Definitions /////////////////////////////////////////

enum axis_enum {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};


///////////////////// Public Function Prototypes ///////////////////////////////

bool get_launch_is_a_go();

// Requests and returns acceleration of axis in (1/2)*m/(s^2)
int16_t accelerometer_read(char axis);

// Returns magnitude of aggregate vector in [(1/2)*m/(s^2)]^2 (to avoid square rooting)
uint32_t acceleration_agg_mag(void);

// Determines whether the rover is right-side-up (true) or upside-down (false).
bool is_up(void);

// Called by timer0 channel B interrupt. Changes launch_is_a_go global variable
void is_launched(uint64_t samples);

#endif
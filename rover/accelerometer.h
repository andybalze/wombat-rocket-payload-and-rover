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

///////////////////// Type Definitions /////////////////////////////////////////

enum axis_enum {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};

///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns acceleration of axis in (1/2)*m/(s^2)
int16_t accelerometer_read(char axis);

// Returns magnitude of agragate vector in (1/2)*m/(s^2)
uint16_t acceleration_agg_mag(void);

// Determines whether the rover is right-side-up (true) or upside-down (false).
bool is_up(void);

#endif
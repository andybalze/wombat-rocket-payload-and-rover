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

///////////////////// Type Definitions /////////////////////////////////////////

enum axis_enum {
    X_AXIS,
    Y_AXIS,
    Z_AXIS
};

///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns acceleration of axis in (1/2)*m/(s^2)
signed int accelerometer_read(char axis);

// Returns magnitude of agragate vector in (1/2)*m/(s^2)
signed int acceleration_agg_mag(void);

#endif
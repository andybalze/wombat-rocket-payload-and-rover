#ifndef _IR_H_
#define _IR_H_

////////////////////////////////////////////////////////////////////////////////
//
// IR
//
// Provides functions that convert ATMega328p's 10-bit ADC value read from
// the GP2Y0E02A IR distance sensor into centimeters.
//
////////////////////////////////////////////////////////////////////////////////

#include "digital_io.h"

///////////////////// Public Function Prototypes ///////////////////////////////

// Initializes IR power pin to an output
void ir_initialize(void);

// Turns on or off the IR sensor.
void ir_power(output_state_t state);

// Requests and returns a distance from the IR sensor in centemeters.
int16_t ir_distance_read(void);

#endif
////////////////////////////////////////////////////////////////////////////////
//
// IR
//
// Provides functions that convert ATMega328p's 10-bit ADC value read from
// the GP2Y0E02A IR distance sensor into centimeters.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include "adc.h"
#include "ir.h"

///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns a distance from the IR sensor in centemeters.
int16_t ir_distance_read(void) {

    // Define variable types
    int16_t ir_distance_cm_comp;                                        // Final deadband compensated distance in centimeters
    int16_t offset = 61;                                                // Equation offset (b)
    int16_t conversion = adc_get_channel_result(ADC_CHANNEL_ADC0)/12;   // Equation linear coefficent (m)

    // Compute distance equation (y=mx+b)
    int16_t ir_distance_cm_uncomp = offset - conversion;

    // Deadband compensation
    if (ir_distance_cm_uncomp >= offset-2)              // If measured distance is within deadband or out of range
    {
        ir_distance_cm_comp = 0;                        // Return compensated value of 0
    }
    else                                                // Else 
    {   
        ir_distance_cm_comp = ir_distance_cm_uncomp;    // Return uncompensated value
    }

    // Return distance in centimeters
    return ir_distance_cm_comp;

}
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

    // Define output type
    signed int ir_distance_adc, distance;
    int16_t ir_distance_cm;

    // Request ADC value of IR distance sensor
    ir_distance_adc = (signed int) adc_get_channel_result(ADC_CHANNEL_ADC0);

    distance = 61 - (ir_distance_adc/12);

    // ADC value to centimeters equation (Value will be truncated )
    ir_distance_cm = (int16_t) distance;  // Verified division works properly, when negative is added it sets to 2's complement, when added the adc read value goes to zero...

    // Return distance in centimeters
    return ir_distance_cm;

}
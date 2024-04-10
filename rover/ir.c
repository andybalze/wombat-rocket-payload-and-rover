////////////////////////////////////////////////////////////////////////////////
//
// IR
//
// Provides functions that convert ATMega328p's 10-bit ADC value read from
// the GP2Y0E02A IR distance sensor into centimeters.
//
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "adc.h"

///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns a distance from the IR sensor in centemeters.
int16_t ir_distance_read(void) {

    // Define output type
    int16_t ir_distance_cm;

    // Request ADC value of IR distance sensor
    ir_distance_adc = adc_get_channel_result(ADC_CHANNEL_ADC0);

    // ADC value to centimeters equation (Value will be truncated )
    ir_distance_cm = -(ir_distance_adc/12)+61;

    // Return distance in centimeters
    return ir_distance_cm;

}
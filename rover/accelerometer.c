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

#include <stdio.h>
#include <math.h>

#include "adc.h"
#include "accelerometer.h"

///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns acceleration of axis in (1/2)*m/(s^2)
signed int accelerometer_read(char axis) {
    signed int accel_returned;

    switch (axis) {
        case X_AXIS:    // If X-axis requested return X-axis value
        accel_returned = adc_get_channel_result(ADC_CHANNEL_ADC2) - 509;
        break;

        case Y_AXIS:    // If Y-axis requested return Y-axis value
        accel_returned = adc_get_channel_result(ADC_CHANNEL_ADC1) - 510;
        break;

        case Z_AXIS:    // If Z-axis requested return Z-axis value
        accel_returned = adc_get_channel_result(ADC_CHANNEL_ADC3) - 512;
        break;

        default:        // Its broken...
        accel_returned = 6969;  // Nice... I guess?
    }

    return accel_returned;
}

// Returns magnitude of agragate vector in (1/2)*m/(s^2)
signed int acceleration_agg_mag(void) {
    signed int mag_returned, x_comp, y_comp, z_comp;

    x_comp = accelerometer_read(X_AXIS);
    y_comp = accelerometer_read(Y_AXIS);
    z_comp = accelerometer_read(Z_AXIS);

    mag_returned = sqrt(pow(x_comp, 2) + pow(y_comp, 2) + pow(z_comp, 2));  // Good old Pythagoreas

    return mag_returned;
}

bool is_up(void) {
    int acceleration_z;
    acceleration_z = accelerometer_read(Z_AXIS);
    if (acceleration_z > 0) {
        return true;
    } else {
        return false;
    }
}
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

#include <math.h>

#include "adc.h"
#include "accelerometer.h"

///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns acceleration of axis in (1/2)*m/(s^2)
int16_t accelerometer_read(char axis) {
    int16_t accel_returned;

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
uint16_t acceleration_agg_mag(void) {
    uint16_t x_comp, y_comp, z_comp;
    int16_t mag_returned;

    x_comp = accelerometer_read(X_AXIS);
    y_comp = accelerometer_read(Y_AXIS);
    z_comp = accelerometer_read(Z_AXIS);

    mag_returned = sqrt(pow(x_comp, 2) + pow(y_comp, 2) + pow(z_comp, 2));  // Good old Pythagoreas

    return mag_returned;
}

bool is_up(void) {
    int16_t acceleration_z;
    acceleration_z = accelerometer_read(Z_AXIS);
    if (acceleration_z > 0) {
        return true;
    } else {
        return false;
    }
}
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

#include "accelerometer.h"
#include "adc.h"
#include "timer.h"

// Don't use ONE_G macro too freely! You'll introduce big rounding errors!
// 1 G is actually 19.6 (1/2)*m/(s^2). I rounded down so is_launched is slightly more sensitive.
#define ONE_G           19          // Use with caution

#define LAUNCH_FORCE    8 * ONE_G

// Sample every 0.1 second for 10 seconds        // TEST // Get a second, third, fourth opinion
#define LAUNCH_SAMPLE_RATE      100
#define LAUNCH_SAMPLE_NUM       100
#define NUM_SAMPLES_ACCEPTABLE  95

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

// Determines whether the rover is right-side-up (true) or upside-down (false).
bool is_up(void) {
    int16_t acceleration_z;
    bool return_val;

    acceleration_z = accelerometer_read(Z_AXIS);
    if (acceleration_z > 0) {
        return_val = true;
    } else {
        return_val = false;
    }

    return return_val;
}

// Returns false if the rocket has not launched. Returns true if the rocket has launched.
// Does not enter function for a substantial amount of time if there are high Gs
bool is_launched(void) {
    uint16_t gamma;         // letter scramble of acc_agg_mag
    uint8_t  high_G_sample_cnt = 0;
    bool return_val = false;

    gamma = acceleration_agg_mag();

    if (gamma > LAUNCH_FORCE) {
        for (int i = 0; i < LAUNCH_SAMPLE_NUM; i++) {
            reset_timer(timer_beta);
            while (get_timer_cnt(timer_beta) < LAUNCH_SAMPLE_RATE);

            gamma = acceleration_agg_mag();

            if (gamma > LAUNCH_FORCE) {
                high_G_sample_cnt++;
            }
        }

        if (high_G_sample_cnt >= NUM_SAMPLES_ACCEPTABLE) {
            return_val = true;
        }
    }

    return return_val;
}
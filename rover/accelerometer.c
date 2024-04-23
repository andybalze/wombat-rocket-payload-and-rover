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

#include "config.h"
#include "accelerometer.h"
#include "adc.h"
#include "timer.h"


///////////////////// Global Variables /////////////////////////////////////////
static volatile bool launch_is_a_go = false;
static volatile bool no_motion = false;

void reset_launch_is_a_go() {
    launch_is_a_go = false;
}

bool get_launch_is_a_go() {
    return launch_is_a_go;
}

void reset_no_motion() {
    no_motion = false;
}

bool get_no_motion() {
    return no_motion;
}


///////////////////// Public Function Prototypes ///////////////////////////////

// Requests and returns acceleration of axis in (1/2)*m/(s^2)
int16_t accelerometer_read(char axis) {
    int16_t accel_returned;

    switch (axis) {
        case X_AXIS:    // If X-axis requested return X-axis value
        accel_returned = adc_get_channel_result(ADC_CHANNEL_ADC2) - 512 + X_AXIS_ERROR;
        break;

        case Y_AXIS:    // If Y-axis requested return Y-axis value
        accel_returned = adc_get_channel_result(ADC_CHANNEL_ADC1) - 512 + Y_AXIS_ERROR;
        break;

        case Z_AXIS:    // If Z-axis requested return Z-axis value
        accel_returned = adc_get_channel_result(ADC_CHANNEL_ADC3) - 512 + Z_AXIS_ERROR;
        break;

        default:        // Its broken...
        accel_returned = 6969;  // Nice... I guess?
    }

    return accel_returned;
}

// Returns magnitude of aggregate vector in (1/2)*m/(s^2)
uint32_t acceleration_agg_mag(void) {
    int32_t x_comp, y_comp, z_comp;
    uint32_t mag_returned;

    x_comp = accelerometer_read(X_AXIS);
    y_comp = accelerometer_read(Y_AXIS);
    z_comp = accelerometer_read(Z_AXIS);

    // mag_returned = sqrt(pow(x_comp, 2) + pow(y_comp, 2) + pow(z_comp, 2));  // Good old Pythagoreas
    mag_returned = (x_comp * x_comp) + (y_comp * y_comp) + (z_comp * z_comp);

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

// Called by timer2 channel A interrupt. Changes launch_is_a_go global variable
void is_launched(logic_vector samples) {
    logic_vector check_vector = 0x1;
    uint8_t high_G_cnt = 0;
    
    for (int i = 0; i < 64; i++) {
        if (samples & check_vector) {
            high_G_cnt++;
        }

        check_vector = check_vector << 1;
    }

    if (high_G_cnt >= LAUNCH_FORCE_CNT_THRESHOLD) {
        launch_is_a_go = true;
    }
}

// Called by timer2 channel B interrupt. Changes no_motion global variable
void is_no_motion(uint64_t samples) {
    logic_vector check_vector = 0x1;
    uint8_t no_motion_cnt = 0;
    
    for (int i = 0; i < 64; i++) {
        if (samples & check_vector) {
            no_motion_cnt++;
        }

        check_vector = check_vector << 1;
    }

    if (no_motion_cnt >= LAUNCH_FORCE_CNT_THRESHOLD) {
        no_motion = true;
    }
}
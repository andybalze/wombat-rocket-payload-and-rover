////////////////////////////////////////////////////////////////////////////////
//
// Current Sensor
//
// Provides functions that return current in the right and left drive motors in 
// milliamps.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "current_sensor.h"

//////////////// Public Function Bodies ////////////////////////////////////////

int16_t motor_current_read(motor_channel_t channel) {

    // Define output
    int16_t motor_current_ma;
    
    // Select motor channel
    switch (channel)
    {
    case RIGHT_CHANNEL:     // Right motor current requested
        motor_current_ma = 3*(adc_get_channel_result(ADC_CHANNEL_ADC7)/7);    // Convert into mA
        break;

    case LEFT_CHANNEL:      // Left motor current requested
        motor_current_ma = 3*(adc_get_channel_result(ADC_CHANNEL_ADC6)/7);    // Convert into mA

    default:        // Error
        motor_current_ma = 0;
        break;
    }

    return motor_current_ma;    // Returns motor current in mA
}
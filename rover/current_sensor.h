#ifndef _CURRENT_SENSOR_H
#define _CURRENT_SENSOR_H

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

///////////////////// Type Definitions /////////////////////////////////////////

// The motor channels available to read
enum motor_channel_enum {
    RIGHT_CHANNEL,
    LEFT_CHANNEL
};

typedef enum motor_channel_enum motor_channel_t;

///////////////////// Public Function Prototypes ///////////////////////////////

int16_t motor_current_read(motor_channel_t channel);

#endif
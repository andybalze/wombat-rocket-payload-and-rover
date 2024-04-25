////////////////////////////////////////////////////////////////////////////////
//
// Rover Main
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

// #include <math.h>   // DEBUG //


#include "config.h"
#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "timer.h"
#include "accelerometer.h"
#include "ir.h"
#include "avoid_obstacles.h"



enum rover_mode_enum {
    RESET,
    MANUAL_LOAD_MODE,
    FLIGHT_MODE
};
typedef enum rover_mode_enum rover_mode_t;

enum flight_state_enum {
    WAIT_FOR_LAUNCH,
    WAIT_FOR_LANDING,
    EXIT_CANISTER,
    DRIVE_FORWARD,
    DISPENSE_DATA_CUBES,
    SIGNAL_ONBOARD_DATA_CUBE,
    DEAD_LOOP
};
typedef enum flight_state_enum flight_state_t;


int main() {

	digital_io_initialize();
	uart_initialize();
	adc_initialize();
    PWM_enable();
	motors_initialize();

    while (1) {
        avoid();
    }
}
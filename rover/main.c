////////////////////////////////////////////////////////////////////////////////
//
// Rover Main Test
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

#include "digital_io.h"
#include "uart.h"
#include "adc.h"
#include "motors.h"
#include "timer.h"
#include "accelerometer.h"

#include "test.h"



int main() {

    digital_io_initialize();                                                        // initialize functions
    uart_initialize();
    adc_initialize();
    motors_initialize();
    timer_initialize();

    test();

    while(1);

}

#include "test.h"
#include "../common/uart.h"
#include "digital_io.h"
#include "motors.h"

void test(void)
{
    uart_transmit_formatted_message("Begin test\r\n");
    UART_WAIT_UNTIL_DONE();
    // enable_soft_start();

    while(1) {
        if (SW_read(LOAD_BTN)) {
            motor(RIGHT_MOTOR, REVERSE, 255);
        }
        else if (SW_read(UNLOAD_BTN)) {
            motor(RIGHT_MOTOR, FORWARD, 255);
        }
        else {
            motor(RIGHT_MOTOR, REVERSE, 0);
        }
    }
}

#include "test.h"
#include "../common/uart.h"
#include "digital_io.h"
#include "motors.h"

#define SPEED 179

void test(void)
{
    uart_transmit_formatted_message("Begin test\r\n");
    UART_WAIT_UNTIL_DONE();

    while(1) {
        if (SW_read(LOAD_BTN)) {
            motor(RIGHT_MOTOR, REVERSE, SPEED);
        }
        else if (SW_read(UNLOAD_BTN)) {
            motor(RIGHT_MOTOR, FORWARD, SPEED);
        }
        else {
            motor(RIGHT_MOTOR, REVERSE, 0);
        }
    }
}

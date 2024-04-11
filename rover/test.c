#include "test.h"
#include "../common/uart.h"
#include "digital_io.h"
#include "motors.h"

void test(void)
{
    uart_transmit_formatted_message("Begin test\r\n");
    enable_soft_start();
    motor(LEFT_MOTOR, REVERSE, 255);

    while(1) {
        
    }
}

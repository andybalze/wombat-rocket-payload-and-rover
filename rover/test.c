#include "test.h"
#include "../common/uart.h"
#include "digital_io.h"
#include "motors.h"

#define SPEED 154

void test(void)
{
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

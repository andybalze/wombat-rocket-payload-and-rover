#include "test.h"
#include "../common/uart.h"
#include "digital_io.h"
#include "motors.h"

#define SPEED 179

void test(void)
{
    motor(RIGHT_MOTOR, FORWARD, 255);
    motor(LEFT_MOTOR, FORWARD, 199);

    while(1) {
    }
}

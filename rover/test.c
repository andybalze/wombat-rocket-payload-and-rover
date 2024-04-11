#include "test.h"
#include "digital_io.h"
#include "motors.h"

void test(void)
{
    char mode;
    char speed1 = 0;
    char speed2 = 0;

    motor(LEFT_MOTOR, FORWARD, speed1);
    motor(RIGHT_MOTOR, FORWARD, speed2);

    while(1) {
        mode = SW_read(ROVER_MODE_SW);

        if (mode == 0) {
            LED_set(RED, ON);
            LED_set(GREEN, ON);
            if (SW_read(LOAD_BTN)) {
                motor(DISPENSER_MOTOR, REVERSE, 255);
            }
            else if (SW_read(UNLOAD_BTN)) {
                motor(DISPENSER_MOTOR, FORWARD, 255);
            }
        }
        else {  // (mode == 1)
            LED_set(RED, OFF);
            LED_set(GREEN, ON);
            if (SW_read(LOAD_BTN) && speed1 != 255) {
                speed1+= 10;
                speed2+= 10;
                motor(LEFT_MOTOR, FORWARD, speed1);
                motor(RIGHT_MOTOR, FORWARD, speed2);
            }
            else if (SW_read(UNLOAD_BTN) && speed1 != 0) {
                speed1-= 10;
                speed2-= 10;
                motor(LEFT_MOTOR, FORWARD, speed1);
                motor(RIGHT_MOTOR, FORWARD, speed2);
            }
        }
    }
}

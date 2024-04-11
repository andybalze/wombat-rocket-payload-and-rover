#include "motors.h"

#define LEFT1_DDR       DDRD
#define LEFT1_PORT      PORTD
#define LEFT1_INDEX     PIN3
#define LEFT1_OCR       OCR2B
#define LEFT1_COM_0     COM2B0
#define LEFT1_COM_1     COM2B1

#define LEFT2_DDR       DDRB
#define LEFT2_PORT      PORTB
#define LEFT2_INDEX     PIN3
#define LEFT2_OCR       OCR2A
#define LEFT2_COM_0     COM2A0
#define LEFT2_COM_1     COM2A1


#define RIGHT1_DDR       DDRD
#define RIGHT1_PORT      PORTD
#define RIGHT1_INDEX     PIN6
#define RIGHT1_OCR       OCR0A
#define RIGHT1_COM_0     COM0A0
#define RIGHT1_COM_1     COM0A1

#define RIGHT2_DDR       DDRD
#define RIGHT2_PORT      PORTD
#define RIGHT2_INDEX     PIN5
#define RIGHT2_OCR       OCR0B
#define RIGHT2_COM_0     COM0B0
#define RIGHT2_COM_1     COM0B1


#define DISPENSER1_DDR       DDRB
#define DISPENSER1_PORT      PORTB
#define DISPENSER1_INDEX     PIN1

#define DISPENSER2_DDR       DDRB
#define DISPENSER2_PORT      PORTB
#define DISPENSER2_INDEX     PIN2


void motors_initialize(void)
{
    // Configure as an outputs
    LEFT1_DDR       |= _BV(LEFT1_INDEX);
    LEFT2_DDR       |= _BV(LEFT2_INDEX);
    RIGHT1_DDR      |= _BV(RIGHT1_INDEX);
    RIGHT2_DDR      |= _BV(RIGHT2_INDEX);
    DISPENSER1_DDR  |= _BV(DISPENSER1_INDEX);
    DISPENSER2_DDR  |= _BV(DISPENSER2_INDEX);

    // Initialize outputs as off
    LEFT1_PORT      &= ~_BV(LEFT1_INDEX);
    LEFT2_PORT      &= ~_BV(LEFT2_INDEX);
    RIGHT1_PORT     &= ~_BV(RIGHT1_INDEX);
    RIGHT2_PORT     &= ~_BV(RIGHT2_INDEX);
    DISPENSER1_PORT &= ~_BV(DISPENSER1_INDEX);
    DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);

    // Select no prescaler
    TCCR2B |= _BV(CS20);    //timer2 (LEFT1 and LEFT2)
    TCCR0B |= _BV(CS00);    //timer0 (RIGHT1 and RIGHT2)

    // Set compare output mode
    TCCR2A |= _BV(LEFT1_COM_1)  | _BV(LEFT2_COM_1);     //Clear OC2A/B on Compare Match, set OC2A/B at BOTTOM for LEFT1 and LEFT2
    TCCR0A |= _BV(RIGHT1_COM_1) | _BV(RIGHT2_COM_1);    //Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM for RIGHT1 and RIGHT2

    // Set waveform generation mode
    TCCR2A |= _BV(WGM21) | _BV(WGM20);  //Fast PWM for LEFT1 and LEFT2
    TCCR0A |= _BV(WGM01) | _BV(WGM00);  //Fast PWM for RIGHT1 and RIGHT2

    LEFT1_OCR  = 0;
    LEFT2_OCR  = 0;
    RIGHT1_OCR = 0;
    RIGHT2_OCR = 0;
}



void motor(motor_name_t motor_name, motor_direction_t direction, char speed)
{
    switch (motor_name) {
        case LEFT_MOTOR:
            if (speed != 0) {
                if (direction == FORWARD) {
                    LEFT1_OCR = speed;
                    LEFT2_OCR = 0;
                }
                else {  // (direction == REVERSE)
                    LEFT1_OCR = 0;
                    LEFT2_OCR = speed;
                }
            }
            else {      // (speed == 0)
                LEFT1_OCR = 0;
                LEFT2_OCR = 0;
            }
            break;

        case RIGHT_MOTOR:

            break;

        case DISPENSER_MOTOR:
            if (speed != 0) {
                if (direction == FORWARD) {
                    DISPENSER1_PORT |=  _BV(DISPENSER1_INDEX);
                    DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);
                }
                else {  // (direction == REVERSE)
                    DISPENSER1_PORT &= ~_BV(DISPENSER1_INDEX);
                    DISPENSER2_PORT |=  _BV(DISPENSER2_INDEX);
                }
            }
            else {      // (speed == 0)
                DISPENSER1_PORT &= ~_BV(DISPENSER1_INDEX);
                DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);
            }
            break;

        default:
            break;
    }
}
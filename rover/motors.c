#include "motors.h"


//////////////////// Macros for Accessing Registers ////////////////////
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


#define RIGHT1_DDR       DDRB
#define RIGHT1_PORT      PORTB
#define RIGHT1_INDEX     PIN1
#define RIGHT1_OCR       OCR1A
#define RIGHT1_COM_0     COM1A0
#define RIGHT1_COM_1     COM1A1

#define RIGHT2_DDR       DDRB
#define RIGHT2_PORT      PORTB
#define RIGHT2_INDEX     PIN2
#define RIGHT2_OCR       OCR1B
#define RIGHT2_COM_0     COM1B0
#define RIGHT2_COM_1     COM1B1


#define DISPENSER1_DDR       DDRD
#define DISPENSER1_PORT      PORTD
#define DISPENSER1_INDEX     PIN6

#define DISPENSER2_DDR       DDRD
#define DISPENSER2_PORT      PORTD
#define DISPENSER2_INDEX     PIN5
//////////////////// Macros for Accessing Registers ////////////////////



//////////////////// Macros for Motor OCR Values ////////////////////
#define OCR_MAX 255     // OCR is in inverting mode so setting to OCR_MAX turns outputs off
//////////////////// Macros for Motor OCR Values ////////////////////



_Noreturn void rover_failure_state(void) {      // For DEBUG. Make this less harsh for flight...
    LED_set(RED, ON);
    LED_set(GREEN, OFF);
    while(1);
}



void motors_initialize(void) {
    //////////////////// Configure Motors ////////////////////
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

    // Select 1024 prescaler (for testing)
    TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);    //timer2 (LEFT1 and LEFT2)
    TCCR1B |= _BV(CS02) | _BV(CS00);                //timer0 (RIGHT1 and RIGHT2)

    // Set waveform generation mode
    TCCR2A |= _BV(WGM21) | _BV(WGM20);  //Fast PWM for LEFT1 and LEFT2
    TCCR1A |= _BV(WGM10);               //Fast PWM for RIGHT1 and RIGHT2
    TCCR1B |= _BV(WGM12);               //also fast PWM for RIGHT1 and RIGHT2

    // Set compare output mode
    TCCR2A |= _BV(LEFT1_COM_1)  | _BV(LEFT1_COM_0)  | _BV(LEFT2_COM_1)  | _BV(LEFT2_COM_0);     //Clear OC2A/B on Compare Match, clear OC2A/B at BOTTOM (inverting mode) for LEFT1 and LEFT2
    TCCR1A |= _BV(RIGHT1_COM_1) | _BV(RIGHT1_COM_0) | _BV(RIGHT2_COM_1) |_BV(RIGHT2_COM_0);     //Clear OC1A/B on Compare Match, clear OC1A/B at BOTTOM (inverting mode) for RIGHT1 and RIGHT2

    // Set output compare register
    LEFT1_OCR  = 255;
    LEFT2_OCR  = 255;
    RIGHT1_OCR = 255;
    RIGHT2_OCR = 255;
    //////////////////// Configure Motors ////////////////////
}



void motor(motor_name_t motor_name, motor_direction_t direction, uint8_t speed) {
    uint8_t ocr_val;

    if (speed < 0) {                // Account for if the control code doesn't like directions
        direction = !direction;
        speed = -speed;
    }

    if (speed > SPEED_MAX) {        // Apply the limit to the motors
        ocr_val = -SPEED_MAX + OCR_MAX;
    }
    else {                          // The function was used correctly
        ocr_val = -speed + OCR_MAX;
    }

    switch (motor_name) {
        case LEFT_MOTOR: {
            if (ocr_val != OCR_MAX) {
                if (direction == FORWARD) {
                    LEFT1_OCR = ocr_val;
                    LEFT2_OCR = OCR_MAX;
                }
                else {  // (direction == REVERSE)
                    LEFT1_OCR = OCR_MAX;
                    LEFT2_OCR = ocr_val;
                }
            }
            else {      // (ocr_val == OCR_MAX)
                LEFT1_OCR = OCR_MAX;
                LEFT2_OCR = OCR_MAX;
            }
            break;
        }

        case RIGHT_MOTOR: {
            if (ocr_val != OCR_MAX) {
                if (direction == FORWARD) {
                    RIGHT1_OCR = OCR_MAX;
                    RIGHT2_OCR = ocr_val;
                }
                else {  // (direction == REVERSE)
                    RIGHT1_OCR = ocr_val;
                    RIGHT2_OCR = OCR_MAX;
                }
            }
            else {      // (ocr_val == OCR_MAX)
                RIGHT1_OCR = OCR_MAX;
                RIGHT2_OCR = OCR_MAX;
            }
            break;
        }

        case DISPENSER_MOTOR: {
            if (ocr_val != OCR_MAX) {
                if (direction == FORWARD) {
                    DISPENSER1_PORT |=  _BV(DISPENSER1_INDEX);
                    DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);
                }
                else {  // (direction == REVERSE)
                    DISPENSER1_PORT &= ~_BV(DISPENSER1_INDEX);
                    DISPENSER2_PORT |=  _BV(DISPENSER2_INDEX);
                }
            }
            else {      // (ocr_val == OCR_MAX)
                DISPENSER1_PORT &= ~_BV(DISPENSER1_INDEX);
                DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);
            }
            break;
        }

        default: {
            uart_transmit_formatted_message("ERROR 734: unknown motor identifier\r\n");    // TEST //
            UART_WAIT_UNTIL_DONE();     // TEST //
            rover_failure_state();
            break;
        }
    }
}

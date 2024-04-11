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


#define SOFT_START_OCR      OCR1A       // timer1 channel A (connected to Dispenser motor pin 1)
#define SOFT_START_COM_0    COM1A0
#define SOFT_START_COM_1    COM1A1
#define SOFT_START_OCIE     OCIE1A
//////////////////// Macros for Accessing Registers ////////////////////



//////////////////// Macros for Soft Start ////////////////////
#define SPEED_MAX 255
#define SPEED_CHANGE 1
//////////////////// Macros for Soft Start ////////////////////



//////////////////// Configure Motors ////////////////////
volatile motor_direction_t left_direction = FORWARD;
volatile motor_direction_t right_direction = FORWARD;
volatile char left_speed = 0;
volatile char right_speed = 0;
//////////////////// Configure Motors ////////////////////



_Noreturn void rover_failure_state(void) {      // For DEBUG. Make this less harsh for flight...
    disable_soft_start();
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

    // Select no prescaler
    TCCR2B |= _BV(CS20);    //timer2 (LEFT1 and LEFT2)
    TCCR0B |= _BV(CS00);    //timer0 (RIGHT1 and RIGHT2)

    // // Select 1024 prescaler (for testing)
    // TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);    //timer2 (LEFT1 and LEFT2)
    // TCCR0B |= _BV(CS02) | _BV(CS00);                //timer0 (RIGHT1 and RIGHT2)

    // Set compare output mode
    TCCR2A |= _BV(LEFT1_COM_1)  | _BV(LEFT1_COM_0)  | _BV(LEFT2_COM_1)  | _BV(LEFT2_COM_0);     //Clear OC2A/B on Compare Match, set OC2A/B at BOTTOM (inverting mode) for LEFT1 and LEFT2
    TCCR0A |= _BV(RIGHT1_COM_1) | _BV(RIGHT1_COM_0) | _BV(RIGHT2_COM_1) |_BV(RIGHT2_COM_0);     //Clear OC0A/B on Compare Match, set OC0A/B at BOTTOM (inverting mode) for RIGHT1 and RIGHT2

    // Set waveform generation mode
    TCCR2A |= _BV(WGM21) | _BV(WGM20);  //Fast PWM for LEFT1 and LEFT2
    TCCR0A |= _BV(WGM01) | _BV(WGM00);  //Fast PWM for RIGHT1 and RIGHT2

    // Set output compare register
    LEFT1_OCR  = 255;
    LEFT2_OCR  = 255;
    RIGHT1_OCR = 255;
    RIGHT2_OCR = 255;
    //////////////////// Configure Motors ////////////////////


    ///////////// Configure Soft Start Interrupt /////////////
    TCCR1B          |= _BV(CS10);                       // Select no prescaler
    TCCR1B          |= _BV(WGM12);                      // Set timer to CTC mode
    SOFT_START_OCR  |= 255;                             // Set the output compare value (1 MHz / (255 + 1) = 3.9 kHz)
    SREG            |= _BV(SREG_I);                     // Enable global interrupts
    ///////////// Configure Soft Start Interrupt /////////////
}



void enable_soft_start(void) {
    TIMSK1 |=  _BV(SOFT_START_OCIE);            // Enable the output compare interrupt
}

void disable_soft_start(void) {
    TIMSK1 &= ~_BV(SOFT_START_OCIE);            // Disable the output compare interrupt
}



void motor(motor_name_t motor_name, motor_direction_t direction, char speed) {
    switch (motor_name) {
        case LEFT_MOTOR: {          // Handled by the soft-start interrupt
            left_direction = direction;
            left_speed = speed;
            break;
        }

        case RIGHT_MOTOR: {         // Handled by the soft-start interrupt
            left_direction = direction;
            left_speed = speed;
            break;
        }

        case DISPENSER_MOTOR: {
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
        }

        default: {
            rover_failure_state();
            break;
        }
    }
}


////////// Soft Start Interrupt Handler //////////
ISR(TIMER1_COMPA_vect) {
    ///// TEST /////
    char rover_orientation = 1;     // This will come from the accelerometer in final code
    char current_limit = 0;         // This will come from the motor current limit code
    ///// TEST /////

    motor_direction_t left_direction_actual, right_direction_actual;
    char left_speed_actual, right_speed_actual;


    if (!current_limit) {
        // Read actual direction and speed
        if (LEFT2_OCR == SPEED_MAX) {                                        // if (actual direction is FORWARD)
            left_direction_actual = FORWARD;
            left_speed_actual = -LEFT1_OCR + SPEED_MAX;
        }
        else {                                                         // else if (actual direction is REVERSE)
            left_direction_actual = REVERSE;
            left_speed_actual = -LEFT2_OCR + SPEED_MAX;
        }

        if (RIGHT2_OCR == SPEED_MAX) {                                        // if (actual direction is FORWARD)
            right_direction_actual = FORWARD;
            right_speed_actual = -RIGHT1_OCR + SPEED_MAX;
        }
        else {                                                         // else if (actual direction is REVERSE)
            right_direction_actual = REVERSE;
            right_speed_actual = -RIGHT2_OCR + SPEED_MAX;
        }


        // Set left_speed_actual and left_direction_actual variables
        if (left_direction == left_direction_actual) {
            if (left_speed_actual < left_speed) {         // if desired direction but want faster
                left_speed_actual += SPEED_CHANGE;
            }
            else if (left_speed_actual > left_speed) {    // if desired direciton but want slower
                left_speed_actual -= SPEED_CHANGE;
            }
            else {                                          // if at desired direction and speed
                // do nothing
            }
        }
        else if (left_speed_actual != 0) {                  // if (left_direction != left_direction_actual && left_speed_actual != 0)
            left_speed_actual -= SPEED_CHANGE;
        }
        else if (left_speed != 0) {                         // if (left_direction != left_direction_actual && left_speed_actual == 0 && left_speed != 0)
            left_direction_actual = !left_direction_actual;
            left_speed_actual += SPEED_CHANGE;
        }
        else {                                              // if not desired direction but desired and actual speed are zero
            // do nothing
        }


        // Set right_speed_actual and right_direction_actual variables
        if (right_direction == right_direction_actual) {
            if (right_speed_actual < right_speed) {         // if desired direction but want faster
                right_speed_actual += SPEED_CHANGE;
            }
            else if (right_speed_actual > right_speed) {    // if desired direciton but want slower
                right_speed_actual -= SPEED_CHANGE;
            }
            else {                                          // if at desired direction and speed
                // do nothing
            }
        }
        else if (right_speed_actual != 0) {                 // if (right_direction != right_direction_actual && right_speed_actual != 0)
            right_speed_actual -= SPEED_CHANGE;
        }
        else if (right_speed != 0) {                        // if (right_direction != right_direction_actual && right_speed_actual == 0 && right_speed != 0)
            right_direction_actual = !right_direction_actual;
            right_speed_actual += SPEED_CHANGE;
        }
        else {                                              // if not desired direction but desired and actual speed are zero
            // do nothing
        }



        // Set left actual direction/speed
        switch (left_direction_actual) {
            case FORWARD: {
                LEFT1_OCR = -left_speed_actual + SPEED_MAX;
                LEFT2_OCR = SPEED_MAX;
                break;
            }

            case REVERSE: {
                LEFT1_OCR = SPEED_MAX;
                LEFT2_OCR = -left_speed_actual + SPEED_MAX;
                break;
            }

            default: {
                rover_failure_state();
                break;
            }
        }

        // Set right actual direction/speed
        switch (right_direction_actual) {
            case FORWARD: {
                RIGHT1_OCR = -right_speed_actual + SPEED_MAX;
                RIGHT2_OCR = SPEED_MAX;
                break;
            }

            case REVERSE: {
                RIGHT1_OCR = SPEED_MAX;
                RIGHT2_OCR = -right_speed_actual + SPEED_MAX;
                break;
            }

            default: {
                rover_failure_state();
                break;
            }
        }

//////////////////// TEST ////////////////////
        uart_transmit_formatted_message("Right speed: %d\r\nLeft speed: %d\r\n\r\n", right_speed_actual, left_speed_actual);
        UART_WAIT_UNTIL_DONE();
//////////////////// TEST ////////////////////
    }
}
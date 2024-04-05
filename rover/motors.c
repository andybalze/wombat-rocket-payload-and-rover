#include "motors.h"


#define LEFT1_DDR      DDRD
#define LEFT1_PORT     PORTD
#define LEFT1_INDEX    PIN3
#define LEFT1_OCR      OCR2B

#define LEFT2_DDR      DDRB
#define LEFT2_PORT     PORTB
#define LEFT2_INDEX    PIN3


#define RIGHT1_DDR      DDRD
#define RIGHT1_PORT     PORTD
#define RIGHT1_INDEX    PIN6
#define RIGHT1_OCR      OCR0A

#define RIGHT2_DDR      DDRD
#define RIGHT2_PORT     PORTD
#define RIGHT2_INDEX    PIN5


#define DISPENSER1_DDR      DDRB
#define DISPENSER1_PORT     PORTB
#define DISPENSER1_INDEX    PIN1
#define DISPENSER1_OCR      OCR1A

#define DISPENSER2_DDR      DDRB
#define DISPENSER2_PORT     PORTB
#define DISPENSER2_INDEX    PIN2


#define SOFTSTART_OCR OCR0A

#define MAX_COUNT 255

static volatile motor_direction_t left_motor_direction = FORWARD;
static volatile motor_direction_t right_motor_direction = FORWARD;
static volatile motor_direction_t dispenser_motor_direction = FORWARD;

static volatile char left_motor_speed = 0;
static volatile char right_motor_speed = 0;
static volatile char dispenser_motor_speed = 0;


void motors_initialize(void)
{
    // Set data direction for output pins
    LEFT1_DDR       |= _BV(LEFT1_INDEX);
    LEFT2_DDR       |= _BV(LEFT2_INDEX);
    RIGHT1_DDR      |= _BV(RIGHT1_INDEX);
    RIGHT2_DDR      |= _BV(RIGHT2_INDEX);
    DISPENSER1_DDR  |= _BV(DISPENSER1_INDEX);
    DISPENSER2_DDR  |= _BV(DISPENSER2_INDEX);

    // Initialize outputs as off
    LEFT1_PORT      &= ~_BV(LEFT1_INDEX);
    LEFT2_PORT      |= _BV(LEFT2_INDEX);
    RIGHT1_PORT     &= ~_BV(RIGHT1_INDEX);
    RIGHT2_PORT     &= ~_BV(RIGHT2_INDEX);
    DISPENSER1_PORT &= ~_BV(DISPENSER1_INDEX);
    DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);

    // // Select the 1/1024 prescaler for all timers
    // TCCR0B |= _BV(CS00) | _BV(CS02);
    // TCCR1B |= _BV(CS10) | _BV(CS12);
    // TCCR2B |= _BV(CS20) | _BV(CS21) | _BV(CS22);

    // Select no prescaler for all the timers
    TCCR0B |= _BV(CS00);
    TCCR1B |= _BV(CS10);
    TCCR2B |= _BV(CS20);

    // Set waveform generation mode to fast PWM (fast PWM is up-counting only)
    TCCR0A |= _BV(WGM00) | _BV(WGM01);
    TCCR1A |= _BV(WGM10);
    TCCR1B |= _BV(WGM12);
    TCCR2A |= _BV(WGM20) | _BV(WGM21);

    // Set compare match behavior for PWM pins
    TCCR0A |= _BV(COM0A1); //LEFT1
    TCCR2A |= _BV(COM2B1); //RIGHT1
    TCCR1A |= _BV(COM1A1); //DISPENSER1
    
    // Enable output compare interrupt used for soft starting motors (timer0 channel A)
    TIMSK0 |= _BV(OCIE0A);

    // Set duty cycle for motors (0-MAX_COUNT). Initialize as off
    LEFT1_OCR = 0;
    RIGHT1_OCR = 255;
    DISPENSER1_OCR = 0;

    // Set interrupt period for motor soft start interrupt
    SOFTSTART_OCR = MAX_COUNT; // 31.4 kHz

    // Enable global interrupts
    SREG |= _BV(SREG_I);
}



void motor(motor_name_t motor_name, motor_direction_t direction, char speed)
{
    switch (motor_name)
    {
        case LEFT_MOTOR:
            left_motor_direction = direction;
            left_motor_speed = speed;
            break;

        case RIGHT_MOTOR:
            right_motor_direction = direction;
            right_motor_speed = speed;
            break;

        case DISPENSER_MOTOR:
            dispenser_motor_direction = direction;
            dispenser_motor_speed = speed;
            break;
    }
}



// This function is only meant to be called from the soft start interrupt handler
void change_motor_speed(motor_name_t motor_name)
{
    ////////// TEMPORARY //////////
    int orientation = 1;         // This line is temporary. Accelerometer code will give this value.
    ////////// TEMPORARY //////////

    motor_direction_t actual_dir, adjusted_dir, desired_dir;
    char actual_speed, adjusted_speed, desired_speed;
    int speed_diff;

    switch (motor_name) {
        case LEFT_MOTOR:
            actual_dir = (LEFT2_PORT & _BV(LEFT2_INDEX)) >> LEFT2_INDEX; // read actual direction from register
            actual_speed = LEFT1_OCR;
            desired_dir = left_motor_direction;
            desired_speed = left_motor_speed;
            break;
        
        case RIGHT_MOTOR:
            actual_dir = (RIGHT2_PORT & _BV(RIGHT2_INDEX)) >> RIGHT2_INDEX; // read actual direction from register
            actual_speed = RIGHT1_OCR;
            desired_dir = right_motor_direction;
            desired_speed = right_motor_speed;
            break;

        case DISPENSER_MOTOR:
            actual_dir = (DISPENSER2_PORT & _BV(DISPENSER2_INDEX)) >> DISPENSER2_INDEX; // read actual direction from register
            actual_speed = DISPENSER1_OCR;
            desired_dir = dispenser_motor_direction;
            desired_speed = dispenser_motor_speed;
            break;
        
        default:
            break;
    }

    // Calculate adjusted direction and adjusted speed //
    adjusted_dir = !(orientation ^ desired_dir);                                        // calculate adjusted direction
    switch (adjusted_dir) {                                                             // switch (adjusted direction)
        case 0:                                                                         //     case (adjusted direction == 0)
            adjusted_speed = desired_speed;                                             //         leave 0-255 scale as is
            break;                                                                      //     end case

        case 1:                                                                         //     case (adjusted direction == 1)
            adjusted_speed = -desired_speed + 255;                                      //         flip 0-255 scale
            break;                                                                      //     end case

        default:
            adjusted_speed = -desired_speed + 255; // it's non-zero so flip the scale I guess
            break;
    }                                                                                   // end switch

    if (actual_dir == adjusted_dir) {                                                   // if (actual direction == adjusted direction)
        speed_diff = adjusted_speed - actual_speed;                                     //     speed difference = adjusted speed - actual speed
        switch (speed_diff) {                                                           //     switch (speed difference)
            case 0:                                                                     //         case (actual speed == adjusted speed)
                // do nothing                                                           //             do nothing
                break;                                                                  //         end case

            case -255 ... -1:                                                           //         case (actual speed < adjusted speed)
                actual_speed++;                                                         //             increment actual speed
                break;                                                                  //         end case
        
            case 1 ... 255:                                                             //         case (actual speed > adjusted speed)
                actual_speed--;                                                         //             decrement actual speed
                break;                                                                  //         end case

            default:
                break;
        }                                                                               //     end switch
    }                                                                                   // end if
    else {                                                                              // else if (actual direction != adjusted direction)
        switch (actual_dir) {                                                           //     switch (actual direction)
            case 0:                                                                     //         case (actual direction == 0)
                if (actual_speed == 0) {                                                //             if (actual speed == 0)    //maybe do something about the motor's deadband here...
                    actual_dir = !actual_dir;                                           //                 actual direction = !actual direction
                    if (actual_speed != adjusted_speed) {                               //                 if (actual speed != adjusted speed)
                        actual_speed++;                                                 //                     increment actual speed
                    }                                                                   //                 end if
                }                                                                       //             end if
                else {                                                                  //             else
                    actual_speed--;                                                     //                 decrement actual speed
                }                                                                       //             end else
                break;                                                                  //         end case

            case 1:                                                                     //         case (actual direction == 1)
                if (actual_speed == MAX_COUNT) {                                        //             if (actual speed == MAX_COUNT)
                    actual_dir = !actual_dir;                                           //                 actual direction = !actual direction
                    if (actual_speed != adjusted_speed) {                               //                 if (actual speed != adjusted speed)
                        actual_speed--;                                                 //                     decrement actual speed
                    }                                                                   //                 end if
                }                                                                       //             end if
                else {                                                                  //             else
                    actual_speed++;                                                     //                 increment actual speed
                }                                                                       //             end else
                break;                                                                  //         end case

            default:
                break;
        }                                                                               //     end switch
    }                                                                                   // end else

    switch (motor_name) {
        case LEFT_MOTOR:
            LEFT1_OCR = actual_speed;
            if (actual_dir == 0) {
                LEFT2_PORT &= ~_BV(LEFT2_INDEX);
            }
            else {
                LEFT2_PORT |= _BV(LEFT2_INDEX);
            }
            break;

        case RIGHT_MOTOR:
            RIGHT1_OCR = actual_speed;
            if (actual_dir == 0) {
                RIGHT2_PORT &= ~_BV(RIGHT2_INDEX);
            }
            else {
                RIGHT2_PORT |= _BV(RIGHT2_INDEX);
            }
            break;

        case DISPENSER_MOTOR:
            DISPENSER1_OCR = actual_speed;
            if (actual_dir == 0) {
                DISPENSER2_PORT &= ~_BV(DISPENSER2_INDEX);
            }
            else {
                DISPENSER2_PORT |= _BV(DISPENSER2_INDEX);
            }
            break;

        default:
            break;
    }
}



// Motor soft start interrupt handler
// ISR(TIMER0_COMPA_vect) {
//     change_motor_speed(LEFT_MOTOR);
//     change_motor_speed(RIGHT_MOTOR);
//     change_motor_speed(DISPENSER_MOTOR);
// }

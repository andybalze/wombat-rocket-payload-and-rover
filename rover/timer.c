#include "timer.h"

static volatile uint32_t counter_alpha_cnt = 0;         // Only timer interrupt allowed to change
static volatile uint32_t counter_beta_cnt = 0;          // Only timer interrupt allowed to change


// Initializes TIMER0 which is used for two seperate counters. Can be used at the same time as Left and Right motor PWM
void timer_counter_initialize(void) {
    TCCR0B |= _BV(CS02);                // Select the 256 prescaler
    TCCR0A |= _BV(WGM01);               // Set timer to CTC mode
    TIMSK0 |= _BV(OCIE0A);              // Enable output compare channel A interrupt

    OCR0A = 31;                         // 1000 Hz interrupt frequency (1 ms)

    SREG   |= _BV(SREG_I);              // Enable global interrupts
}



void reset_timer_counter(counter_name_t counter) {
    cli();                      // Disable interrupts
    switch (counter) {
        case counter_alpha: {
            counter_alpha_cnt = 0;
            break;
        }

        case counter_beta: {
            counter_beta_cnt = 0;
            break;
        }

        default: {
            LED_set(RED, ON);
            LED_set(GREEN, OFF);
            uart_transmit_formatted_message("ERROR 158: error resetting timer counter\r\n");
            UART_WAIT_UNTIL_DONE();
            break;
        }
    }
    sei();                      // Enable interrupts
}



uint32_t get_timer_counter(counter_name_t counter) {
    cli();                      // Disable interrupts
    unsigned long int count;

    switch (counter) {
        case counter_alpha: {
            count = counter_alpha_cnt;
            break;
        }

        case counter_beta: {
            count = counter_beta_cnt;
            break;
        }

        default: {
            LED_set(RED, ON);
            LED_set(GREEN, OFF);
            uart_transmit_formatted_message("ERROR 159: error getting timer count\r\n");
            UART_WAIT_UNTIL_DONE();
            break;
        }
    }

    sei();                      // Enable interrupts
    return count;
}



// Enables timer2 channel A interrupt used to sample acceleration in is_launched() (accelerometer.c)
// Cannot use the timer counter while launch check is enabled
void launch_check_enable(void) {
    TCCR2B |= _BV(CS22) | _BV(CS21);    // Select the 256 prescaler
    TCCR2A |= _BV(WGM21);               // Set timer to CTC mode
    TIMSK2 |= _BV(OCIE2A);              // Enable output compare channel A interrupt

    OCR0A = 244;                         // 128 Hz interrupt frequency (approx. 64 samples in 0.5 s)

    SREG   |= _BV(SREG_I);              // Enable global interrupts
}



// Disables timer2 channel A interrupt
void launch_check_disable(void) {
    TIMSK2 &= ~_BV(OCIE2A);              // Disable output compare channel A interrupt
    timer_counter_initialize();
}



// Enables timer2 channel B interrupt. Cannot function if either PWM_enable (from motors.c) or launch_check_enable are called
void no_motion_check_enable(void) {
    TCCR2B |= _BV(CS22) | _BV(CS21);    // Select the 256 prescaler
    TCCR2A |= _BV(WGM21);               // Set timer to CTC mode
    TIMSK2 |= _BV(OCIE2B);              // Enable output compare channel B interrupt

    OCR0A = 244;                         // 128 Hz interrupt frequency (approx. 64 samples in 0.5 s)

    SREG   |= _BV(SREG_I);              // Enable global interrupts
}



// Disables timer2 channel B interrupt
void no_motion_check_disable(void) {
    TIMSK2 &= ~_BV(OCIE2B);              // Disable output compare channel A interrupt
    timer_counter_initialize();
}



////////////////////////////////////////////////////////////////////////////////////////////
////////// Interrupt Service Routines //////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////

// Interrupt service routine used for 1 ms counters
ISR(TIMER0_COMPA_vect) {
    counter_alpha_cnt++;                    // Overflows after 4,294,967,296 ms which is about 50 days
    counter_beta_cnt++;
}



// Interrupt service routine used for launch check
ISR(TIMER2_COMPA_vect) {
    static logic_vector G_force_samples = 0;        // Don't let it's appearance fool you, this is a bool array[64]
    uint32_t gamma;                             // acceleration aggragate magnitude squared

    gamma = acceleration_agg_mag();             // remember, this is magnitude squared

    G_force_samples = G_force_samples << 1;         // shift bottom half of array
    if (gamma >= LAUNCH_FORCE_SQUARED) {
        G_force_samples += 1;                     // insert a 1 into the LSB of bottom half
    }
    else {
        // insert a zero (happens by definition)
    }

    is_launched(G_force_samples);                                // check array to see if we've launched
}



// Interrupt service routine used for launch check
ISR(TIMER2_COMPB_vect) {
    static logic_vector G_force_samples = 0;    // Don't let it's appearance fool you, this is a bool array[64]
    uint32_t gamma;                             // acceleration aggragate magnitude squared

    gamma = acceleration_agg_mag();             // remember, this is magnitude squared

    G_force_samples = G_force_samples << 1;         // shift bottom half of array
    if (gamma >= (ONE_G_SQUARED - NO_MOVEMENT_TOLERANCE_SQUARED) && gamma <= (ONE_G_SQUARED + NO_MOVEMENT_TOLERANCE_SQUARED)) {
        G_force_samples += 1;                     // insert a 1 into the LSB of bottom half
    }
    else {
        // insert a zero (happens by definition)
    }

    is_no_motion(G_force_samples);                                // check array to see if we've launched
}

////////// Interrupt Service Routines //////////////////////////////////////////////////////

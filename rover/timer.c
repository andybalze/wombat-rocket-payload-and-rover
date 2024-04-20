#include "timer.h"

uint32_t timer_alpha_cnt = 0;       // Only timer interrupt allowed to change
uint32_t timer_beta_cnt = 0;       // Only timer interrupt allowed to change



void timer_initialize(void) {
    TCCR0B |= _BV(CS02);                // Select the 256 prescaler
    TCCR0A |= _BV(WGM01);               // Set timer to CTC mode
    TIMSK0 |= _BV(OCIE0A);              // Enable output compare channel A interrupt
    // Channel B interrupt enabled/disabled in (en/dis)able_launch_check() functions

    OCR0A = 31;                         // 1000 Hz interrupt frequency (1 ms)
    OCR0B = 244;                        // 128 Hz interrupt frequency (64 samples in 0.5 s)

    SREG   |= _BV(SREG_I);              // Enable global interrupts
}



void reset_timer(timer_name_t timer) {
    switch (timer) {
        case timer_alpha: {
            timer_alpha_cnt = 0;
            break;
        }

        case timer_beta: {
            timer_beta_cnt = 0;
            break;
        }

        default: {
            LED_set(RED, ON);
            LED_set(GREEN, OFF);
            uart_transmit_formatted_message("ERROR 158: error reseting timer\r\n");    // TEST //
            UART_WAIT_UNTIL_DONE();     // TEST //
            while(1);               // DEBUG //
            break;
        }
    }
}



uint32_t get_timer_cnt(timer_name_t timer) {
    unsigned long int count;

    switch (timer) {
        case timer_alpha: {
            count = timer_alpha_cnt;
            break;
        }

        case timer_beta: {
            count = timer_beta_cnt;
            break;
        }

        default: {
            LED_set(RED, ON);
            LED_set(GREEN, OFF);
            uart_transmit_formatted_message("ERROR 159: error getting timer count\r\n");    // TEST //
            UART_WAIT_UNTIL_DONE();     // TEST //
            while(1);               // DEBUG //
            break;
        }
    }

    return count;
}



// Enables t imer0 channel b interrupt
void enable_launch_check(void) {
    TIMSK0 |= _BV(OCIE0B);           // Enable output compare channel B interrupt
}


// Disables timer0 channel b interrupt
void disable_launch_check(void) {
    TIMSK0 &= ~_BV(OCIE0B);           // Disable output compare channel B interrupt
}



ISR(TIMER0_COMPA_vect) {
    timer_alpha_cnt++;                    // Overflows after 4,294,967,296
    timer_beta_cnt++;
}

ISR(TIMER0_COMPB_vect) {
    static uint64_t G_force_samples = 0;        // Don't let it's appearance fool you, this is a bool array[128]
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
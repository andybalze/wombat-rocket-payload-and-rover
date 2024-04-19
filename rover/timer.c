#include "timer.h"

uint32_t timer_alpha_cnt = 0;       // Only timer interrupt allowed to change
uint32_t timer_beta_cnt = 0;       // Only timer interrupt allowed to change

void timer_initialize(void) {
    TCCR0B |= _BV(CS02);                // Select the 256 prescaler
    TCCR0A |= _BV(WGM01);               // Set timer to CTC mode
    TIMSK0 |= _BV(OCIE0A);              // Enable output compare channel A interrupt
    // TIMSK0 |= _BV(OCIE0B);           // Enable output compare channel B interrupt

    OCR0A = 31;                        // 1000 Hz interrupt frequency (1 ms)
    // OCR0B = 31;

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



ISR(TIMER0_COMPA_vect) {
    timer_alpha_cnt++;                    // Overflows after 4,294,967,296. At 30.6 Hz, that gives us 1,624 days...
    timer_beta_cnt++;
}

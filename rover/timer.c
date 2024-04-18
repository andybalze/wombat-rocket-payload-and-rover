#include "timer.h"

unsigned long int timerA_cnt = 0;       // Only timer interrupt allowed to change
unsigned long int timerB_cnt = 0;       // Only timer interrupt allowed to change

void timer_initialize(void) {
    TCCR0B |= _BV(CS02) | _BV(CS00);    // Select the 1024 prescaler
    TCCR0A |= _BV(WGM01);               // Set timer to CTC mode
    TIMSK0 |= _BV(OCIE0A);              // Enable output compare channel A interrupt
    // TIMSK0 |= _BV(OCIE0B);           // Enable output compare channel B interrupt

    OCR0A = 255;                        // 30.6 Hz interrupt frequency
    OCR0B = 255;                        // 30.6 Hz interrupt frequency

    SREG   |= _BV(SREG_I);              // Enable global interrupts
}



void reset_timer(timer_name_t timer) {
    switch (timer) {
        case timerA: {
            timerA_cnt = 0;
            break;
        }

        case timerB: {
            timerB_cnt = 0;
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



unsigned long int get_timer_cnt(timer_name_t timer) {
    unsigned long int count;

    switch (timer) {
        case timerA: {
            count = timerA_cnt;
            break;
        }

        case timerB: {
            count = timerB_cnt;
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
    timerA_cnt++;                    // Overflows after 4,294,967,296. At 30.6 Hz, that gives us 1,624 days...
}



ISR(TIMER0_COMPB_vect) {
    timerB_cnt++;                    // Overflows after 4,294,967,296. At 30.6 Hz, that gives us 1,624 days...
}
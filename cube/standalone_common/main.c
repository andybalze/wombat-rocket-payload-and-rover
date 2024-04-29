////////////////////////////////////////////////////////////////////////////////
//
// Data Cube Main
//
////////////////////////////////////////////////////////////////////////////////

// Common includes
#include "uart.h"
#include "spi.h"

// Cube common includes
#include "cube_parameters.h"
#include "digital_io.h"
#include "trx.h"
#include "networking_constants.h"
#include "timer.h"
#include "log.h"

// Specific to this cube includes
#include "address.h"
#include "application.h"

// AVR includes
#include <util/delay.h> // Must come after #include "cube_parameters.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// C Standard Library includes
#include <stdlib.h>

/////////////////// Private Defines ///////////////////////////////////////////

// If this macro is defined, UART functionality will be included.
// #define DEBUG_MODE

// The number of milliseconds to wait after the cube is turned on to enter the 
// LOADING state.
#define STARTUP_DURATION_MS 1000
// Wait one full second.

// The number of milliseconds that the data cube's limit switch must be
// depressed for before the cube enters the LOADED state.
#define LOADING_DURATION_MS 5000
// Wait two full seconds (too short?).

// The number of seconds the data cube waits until it turns off its LED.
#define LOADED_1_DURATION_S 10

// The number of milliseconds that the data cube waits after it is dispensed
// from the rover (its limit switch is released) before beginning its RF
// operations. During this time, the data cube's RF hardware is brough online.
#define DISPENSING_DURATION_MS 1000
// Wait 1/10 seconds for the trx to start up.

// These colors are for the state machine leading up to network operation.
// Go to application.c to see how colors are handled during network operation.
#define LED_COLOR_STARTUP               LED_OFF
#define LED_COLOR_READY_TO_LOAD         LED_GREEN
#define LED_COLOR_LOADING               LED_YELLOW
#define LED_COLOR_LOADED_1              LED_RED
#define LED_COLOR_LOADED_2              LED_OFF
#define LED_COLOR_DISPENSING            LED_BLUE

/////////////////// Private Typedefs ///////////////////////////////////////////

// The states that the data cube state machine can be in
enum data_cube_state_enum {

    // The cube waits in this state for a certain amount of time before moving to
    // the READY_TO_LOAD state.
    STARTUP,

    // In this state, the data cube watches for the limit switch be be depressed,
    // which indicates that the data cube (maybe) is being loaded into the
    // dispenser.
    READY_TO_LOAD,

    // In this state, the data cube watches for either 1) the limit switch to be
    // released, in which case the cube wasn't loaded properly and returns to the
    // READY_TO_LOAD state, or 2) a certain amount of time passes without the
    // limit switch being released, which indicates that the data cube has been
    // loaded into the dispenser.
    LOADING,

    // The data cube waits in the dispenser to be dispensed, which is signalled
    // by the limit switch being released.
    LOADED,

    // The data cube has been dispensed onto the ground and waits there while the
    // mission hardware (the transceiver) powers up.
    DISPENSING,

    // The data cube performs its mission functions. Once the data cube enters
    // this state, it will never leave it until the data cube is switched off.
    OPERATIONAL

};
typedef enum data_cube_state_enum data_cube_state_t;

/////////////////// Private Global Variables ///////////////////////////////////

// The current state of this data cube.
volatile static data_cube_state_t current_state = STARTUP;

/////////////////// Private Function Prototypes ///////////////////////////////

// The code that's run each time around the state machine loop, depending on
// the state machine's current state. These functions handle any behavior of
// their states, as well as transitions to any other states.
void state_code_startup(void);
void state_code_ready_to_load(void);
void state_code_loading(void);
void state_code_loaded(void);
void state_code_dispensing(void);
void state_code_operational(void);

/////////////////// Public Function Bodies ////////////////////////////////////

int main() {

    init_log();

    digital_io_initialize();
    uart_initialize();

    LED_set(LED_WHITE);
    uart_transmit_formatted_message("\r\n::: Data Cube %02x :::\r\n", MY_NETWORK_ADDR);
    print_log();

    timer_start(STARTUP_DURATION_MS);
    current_state = STARTUP;

    // The main state machine loop.
    while(1) {
        switch (current_state)
        {
        case STARTUP:
            state_code_startup();
            break;

        case READY_TO_LOAD:
            state_code_ready_to_load();
            break;

        case LOADING:
            state_code_loading();
            break;

        case LOADED:
            state_code_loaded();
            break;

        case DISPENSING:
            state_code_dispensing();
            break;

        case OPERATIONAL:
            // Since a data cube should never leave the OPERATIONAL state, having an
            // infinite loop wrap this function call makes things a little faster.
            while(1) {
                state_code_operational();
            }
            break;

        default:
            break;
        }
    }

}

/////////////////// Private Function Bodies ////////////////////////////////////

void state_code_startup(void) {

    if ((TIFR1 & _BV(OCF1A)) != 0) {

        timer_stop();
        current_state = READY_TO_LOAD;
        LED_set(LED_COLOR_READY_TO_LOAD);

    }

}

void state_code_ready_to_load(void) {

    if (SW_read(SW1)) {

        timer_start(LOADING_DURATION_MS);
        current_state = LOADING;
        LED_set(LED_COLOR_LOADING);

    }

}

void state_code_loading(void) {

    if (!SW_read(SW1)) {

        timer_stop();
        current_state = READY_TO_LOAD;
        LED_set(LED_COLOR_READY_TO_LOAD);

    }

    else if ((TIFR1 & _BV(OCF1A)) != 0) {

        timer_stop();
        timer_start(1000);
        current_state = LOADED;
        LED_set(LED_COLOR_LOADED_1);

    }

}

void state_code_loaded(void) {

    static int seconds_passed;

    if ((TIFR1 & _BV(OCF1A)) != 0) {
        timer_stop();
        seconds_passed = seconds_passed + 1;
        if (seconds_passed >= LOADED_1_DURATION_S) {
            LED_set(LED_COLOR_LOADED_2);
        } else {
            timer_start(1000);
        }
    }

    if (!SW_read(SW1)) {
        timer_start(DISPENSING_DURATION_MS);
        current_state = DISPENSING;
        trx_initialize(MY_DATA_LINK_ADDR);
        LED_set(LED_COLOR_DISPENSING);
    }

}

void state_code_dispensing(void) {

    if ((TIFR1 & _BV(OCF1A)) != 0) {
        timer_stop();
        current_state = OPERATIONAL;
    }

}

void state_code_operational(void) {

    application();

}

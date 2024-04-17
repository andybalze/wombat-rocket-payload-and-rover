////////////////////////////////////////////////////////////////////////////////
//
// Test Launch Data Cube Main
//
// This file provides the main functions for the standalone data cube 0. This
// version of the main code is to be used for the April 20 test launch. It
// demonstrates a bare-bones version of the actual mission.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "address.h"
#include "digital_io.h"
#include "uart.h"
#include "spi.h"
#include "trx.h"

/////////////////// Private Defines ///////////////////////////////////////////

// If this macro is defined, UART functionality will be included.
#define DEBUG_MODE

// The number of milliseconds to wait after the cube is turned on to enter the 
// LOADING state.
#define STARTUP_DURATION_MS 1000
// Wait one full second.

// The number of milliseconds that the data cube's limit switch must be
// depressed for before the cube enters the LOADED state.
#define LOADING_DURATION_MS	2000
// Wait two full seconds (too short?).

// The number of seconds the data cube waits until it turns off its LED.
#define LOADED_1_DURATION_S	60

// The number of milliseconds that the data cube waits after it is dispensed
// from the rover (its limit switch is released) before beginning its RF
// operations. During this time, the data cube's RF hardware is brough online.
#define DISPENSING_DURATION_MS 100
// Wait 1/10 seconds for the trx to start up.

#define LED_COLOR_STARTUP 				0	// Off
#define LED_COLOR_READY_TO_LOAD		2 // Green
#define LED_COLOR_LOADING					2	// Green
#define LED_COLOR_LOADED_1				1	// Red
#define LED_COLOR_LOADED_2				0	// Off
#define LED_COLOR_DISPENSING			4	// Blue

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

// Starts the 16-bit Timer 1 and sets it to run for a certain number of
// milliseconds. Function can check whether the timer is up by checking the
// TIFR1 OCF1A bit. The stop_timer function must then be called.
void start_timer(uint16_t milliseconds);

// Stops the 16-bit Timer 1.
void stop_timer();

/////////////////// Public Function Bodies ////////////////////////////////////

int main() {

	digital_io_initialize();

	start_timer(STARTUP_DURATION_MS);
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
		stop_timer();
		current_state = READY_TO_LOAD;
		LED_set(LED_COLOR_READY_TO_LOAD);
	}

}

void state_code_ready_to_load(void) {

	if (SW_read(SW1)) {
		start_timer(LOADING_DURATION_MS);
		current_state = LOADING;
		LED_set(LED_COLOR_LOADING);
	}

}

void state_code_loading(void) {

	if (!SW_read(SW1)) {
		stop_timer();
		current_state = READY_TO_LOAD;
		LED_set(LED_COLOR_READY_TO_LOAD);
	}

	else if ((TIFR1 & _BV(OCF1A)) != 0) {
		stop_timer();
		start_timer(1000);
		current_state = LOADED;
		LED_set(LED_COLOR_LOADED_1);
	}

}

void state_code_loaded(void) {

	static int seconds_passed;

	if ((TIFR1 & _BV(OCF1A)) != 0) {
		stop_timer();
		seconds_passed = seconds_passed + 1;
		if (seconds_passed >= LOADED_1_DURATION_S) {
			LED_set(LED_COLOR_LOADED_2);
		} else {
			start_timer(1000);
		}
	}

	if (!SW_read(SW1)) {
		start_timer(DISPENSING_DURATION_MS);
		current_state = DISPENSING;
		trx_initialize(MY_DATA_LINK_ADDR);
		LED_set(LED_COLOR_DISPENSING);
	}

}

void state_code_dispensing(void) {

	if ((TIFR1 & _BV(OCF1A)) != 0) {
		stop_timer();
		current_state = OPERATIONAL;
		LED_set(0);
	}

}

void state_code_operational(void) {

	static int received_message_count;

	trx_receive_payload(NULL);
	received_message_count = received_message_count + 1;
	LED_set(received_message_count & 7);

}

void start_timer(uint16_t milliseconds) {

	// Sets the timer count to 0.
	TCNT1 = 0;

	// Sets the compare value.
	OCR1A = milliseconds << 3;

	// Starts running the timer in CTC-OCR1A mode from the 1/1024 prescaler.
	TCCR1B = (_BV(WGM12) | _BV(CS12) | _BV(CS10));

}

void stop_timer(void) {
	TCCR1B = 0;
}

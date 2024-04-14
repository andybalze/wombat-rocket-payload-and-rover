/*

        WARNING

        This is a SIMULATION FILE.
        Do not compile this code for hardware.

*/


#include "sim_trx.h"
#include "sim_delay.h"
#include <stdio.h>

#define THIS_CUBE_RX_ADDRESS 0x0B0B0B0B
#define TARGET_RX_ADDRESS 0x0A0A0A0A

static trx_payload_element_t *example_payload1 = "Hello, world";

static trx_payload_element_t received_payload[TRX_PAYLOAD_LENGTH + 1];

int main() {

    printf("Powering on...\n");

	trx_initialize(THIS_CUBE_RX_ADDRESS);
	_delay_ms(300);


    for (int i = 0; i < TRX_PAYLOAD_LENGTH + 1; i++) {
        received_payload[i] = '\0';
    }

    while(1) {
        printf("Attempting to receive payload.\n");
        trx_receive_payload(received_payload);
        printf("Got a payload: %s\n", received_payload);
        _delay_ms(100);
    }

}

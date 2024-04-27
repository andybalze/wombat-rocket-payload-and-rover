/*
        WARNING

        This is a SIMULATION FILE.
        Do not compile this code for hardware.
*/


#include "sim_trx.h"
#include "sim_delay.h"
#include "network.h"
#include "address.h"
#include <stdio.h>

#define TARGET_NETWORK_ADDR 0x0A

static trx_payload_element_t *example_payload1 = "Hello, world";

static trx_payload_element_t received_payload[TRX_PAYLOAD_LENGTH + 1];

int main() {

    printf("Rover Powering on...\n");

	trx_initialize(MY_DATA_LINK_ADDR);
	_delay_ms(300);


    for (int i = 0; i < TRX_PAYLOAD_LENGTH + 1; i++) {
        received_payload[i] = '\0';
    }

    while(1) {
        printf("Attempting to transmit payload.\n");
        network_tx(example_payload1, 12, TARGET_NETWORK_ADDR, MY_NETWORK_ADDR);
        printf("Transmitted the payload.\n\n");
        _delay_ms(1000);
    }

}

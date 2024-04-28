/*
        WARNING

        This is a SIMULATION FILE.
        Do not compile this code for hardware.
*/


#include "sim_trx.h"
#include "sim_delay.h"
#include "transport.h"
#include "address.h"
#include <stdio.h>

#define TARGET_PORT 0x0A

#define EXAMPLE_PAYLOAD_LEN (120)
static char example_payload[256] = "Hello, world! Here is a message that is too large to fit inside of a single packet. This message is 120 characters long.";

int main() {

    printf("::: Transport layer test :::\n");
    printf("::: Simulating rover     :::\n\n");

	trx_initialize(MY_DATA_LINK_ADDR);
	_delay_ms(300);

    printf("Attempting to transmit payload...\n ");
    fflush(stdout);
    if (transport_tx(example_payload, EXAMPLE_PAYLOAD_LEN, TARGET_PORT)) {
        printf("Succeeded.\n");
    }
    else {
        printf("Failed.\n");
    }
}

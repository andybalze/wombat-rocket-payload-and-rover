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

#define EXAMPLE_PAYLOAD_LEN (353)
static char example_payload[65536] = "Hello, world! Here is a message that is too large to fit inside of a single packet.\nIf you really think about it, when we get home,\nwe can have all the bread we want.\n\nEverything should be made as simple as possible,\nbut no simpler.\n\n\t-Some smart man\n\nWow.... I am really struggling to go beyond 256 characters.\nBut I need to, to test if our code works.";

int main() {

    printf("::: Transport layer test :::\n");
    printf("::: Simulating rover     :::\n\n");

	trx_initialize(MY_DATA_LINK_ADDR);
	_delay_ms(300);

    printf("Attempting to transmit payload... ");
    fflush(stdout);
    if (transport_tx(example_payload, EXAMPLE_PAYLOAD_LEN, TARGET_PORT)) {
        printf("Succeeded.\n");
    }
    else {
        printf("Failed.\n");
    }
}

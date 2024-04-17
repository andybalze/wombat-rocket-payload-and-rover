/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include "address.h"
#include "digital_io.h"
#include "trx.h"

#define TOTAL_TRANSMISSION_COUNT (7)

#define TARGET_ADDRESS (0x0a0a0a0a)

static trx_payload_element_t *payload = "You will not go to space today.";

int main() {

	digital_io_initialize();
	LED_set(0);

	// Wait until the rover instructs the cube to start transmitting.
	while(!SW_read(SW1));

	trx_initialize(MY_DATA_LINK_ADDR);

	int transmission_index = 0;
	while (transmission_index < TOTAL_TRANSMISSION_COUNT) {
		trx_transmit_payload(TARGET_ADDRESS, payload, 32);
		transmission_index = transmission_index + 1;
		LED_set(transmission_index);
	}

	while(1);

}

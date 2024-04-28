/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include "address.h"
#include "digital_io.h"
#include "trx.h"
#include "application.h"

#include "cube_parameters.h"
#include <util/delay.h>

int main() {

	digital_io_initialize();
	LED_set(LED_OFF);

	// Wait until the rover instructs the cube to start transmitting.
	while(SW_read(SW1));

	trx_initialize(MY_DATA_LINK_ADDR);

	application();

	while(1);
}
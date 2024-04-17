/* * * * * * * * * * * * * * *
      Data Cube Software
 * * * * * * * * * * * * * * */

#include "digital_io.h"

#ifndef F_CPU
#define F_CPU 8000000 //8 MHz
#endif

int main() {

	// Setup
	digital_io_initialize();

	// Loop
	while(1);

}

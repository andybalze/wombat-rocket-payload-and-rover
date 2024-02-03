/* * * * * * * * * * * * * * *
         Rover Software
 * * * * * * * * * * * * * * */

#include "test1.h"

#ifndef F_CPU
#define F_CPU 8000000 //8 MHz
#endif

int main() {

	// Setup
	int result = test1(1, 2);

	// Loop
	while(1);

}

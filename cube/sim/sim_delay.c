#include "sim_delay.h"

#include <stdio.h>
#include <unistd.h>

// Note: only has a resolution of 1 second. Sorry
void _delay_ms(int ms) {
    sleep(((float) ms)/1000.0);
}

#include "sim_delay.h"

#include <stdio.h>
#include <unistd.h>

void _delay_ms(int ms) {
    sleep(((float) ms)/1000.0);
}

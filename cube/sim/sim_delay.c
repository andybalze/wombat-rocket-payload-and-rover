#include "sim_delay.h"

#include <stdio.h>
#include <unistd.h>

void _delay_ms(int ms) {
    usleep(ms*1000);
}

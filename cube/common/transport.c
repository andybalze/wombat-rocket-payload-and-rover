#include "networking_constants.h"
#include "transport.h"
#include "network.h"

int transport_rx(char* buffer, int buf_len) {

    // Do some stuff.. as part of this, call network_rx().

    int segment_len;

    char segment[MAX_SEGMENT_LEN];
    segment_len = network_rx(segment, MAX_SEGMENT_LEN);
}
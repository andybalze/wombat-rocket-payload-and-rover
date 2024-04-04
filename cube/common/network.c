#include "networking_constants.h"
#include "network.h"
#include "data_link.h"

int network_rx(char* buffer, int buf_len) {

    // Do some stuff.
    // As part of this, call this function...

    int packet_len;
    char packet[MAX_PACKET_LEN];
    packet_len = data_link_rx(packet, MAX_SEGMENT_LEN);
}

int netork_tx(char* buffer, int buf_len) {

    // Implementation TBD

}

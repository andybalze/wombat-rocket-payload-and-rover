#include "address_resolution.h"
#include "network.h"
#include "data_link.h"
#include "address.h"
#include "routing_table.h"

// packet[0] = length of packet
// packet[1] = final destination network address
// packet[2] = original source network address
// rest is payload

// This blocking function gets a payload from the network layer
// and writes it to the buffer.
// It returns the length of the payload (i.e. the segment).
byte network_rx(byte* buffer, byte buf_len) {

    byte packet_len;
    byte packet[MAX_PACKET_LEN];

    // Repeat this until we get something that's for me.
    do {

        data_link_rx(packet, MAX_PACKET_LEN);
        packet_len = packet[0];

        // If packet isn't for me, forward it along.
        if (packet[1] != MY_NETWORK_ADDR) {
            network_tx(packet, packet_len, packet[1], packet[2]);
        }
        

    } while (packet[1] != MY_NETWORK_ADDR);

    // Got something for me. Let's return it.
    for (byte i = 0; i < packet_len - PACKET_HEADER_LEN && i < buf_len; i++) {
        buffer[i] = packet[i + PACKET_HEADER_LEN];
    }

    return packet_len - PACKET_HEADER_LEN;
}

// Transmit to the specified network address.
void network_tx(byte* payload, byte payload_len, byte dest_network_addr, byte src_network_addr) {

    byte packet_len = payload_len + PACKET_HEADER_LEN;
    byte packet[packet_len];

    packet[0] = packet_len;
    packet[1] = dest_network_addr;
    packet[2] = src_network_addr;
    for (byte i = 0; i < payload_len && i < MAX_PACKET_LEN - PACKET_HEADER_LEN; i++) {
        packet[i + PACKET_HEADER_LEN] = payload[i];
    }
    byte next_hop_addr = routing_table(dest_network_addr);

    data_link_tx(packet, packet_len, resolve_data_link_addr(next_hop_addr));

    return;
}

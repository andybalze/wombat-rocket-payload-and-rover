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
// It returns whether it successfully received a packet.
// Note that the timeout will reset every time it receives a packet.
// This behavior is as such because the programmer is lazy.
bool network_rx(byte* buffer, byte buf_len, uint16_t timeout_ms) {

    byte packet_len;
    byte packet[MAX_PACKET_LEN];

    bool success;
    bool endloop = false;

    // Repeat this until we get something that's for me,
    // or until one of our things times out.
    do {

        success = data_link_rx(packet, MAX_PACKET_LEN, timeout_ms);
        if (success) {
            packet_len = packet[0];
            // If packet isn't for me, forward it along.
            if (packet[1] != MY_NETWORK_ADDR) {
                network_tx(packet, packet_len, packet[1], packet[2]);
            }
            else {
                // Break if we receive a packet for us
                endloop = true;
            }
        }
        else {
            // Break if we time out
            endloop = true;
        }

    } while (endloop);

    // Got something for me. Let's return it.
    if (success) {
        for (byte i = 0; i < packet_len - PACKET_HEADER_LEN && i < buf_len; i++) {
            buffer[i] = packet[i + PACKET_HEADER_LEN];
        }
    }

    return success;
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

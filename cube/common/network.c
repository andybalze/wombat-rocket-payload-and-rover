#include "address_resolution.h"
#include "network.h"
#include "data_link.h"
#include "address.h"
#include "routing_table.h"
#include "digital_io.h"

#ifndef SIMULATION
#include "cube_parameters.h"
#include "print_data.h"
#include "uart.h"
#include <util/delay.h>
#else
#include "sim_delay.h"
#include <stdio.h>
#include "sim_print_data.h"
#endif

#define NETWORK_DELAY_MS (1)

// packet[0] = length of packet
// packet[1] = final destination network address
// packet[2] = original source network address
// rest is payload

// This blocking function gets a payload from the network layer
// and writes it to the buffer.
// It returns whether it successfully received a packet.
// Note that the timeout will reset every time it receives a packet.
// This behavior is as such because the programmer is lazy.
network_rx_result network_rx(byte* buffer, byte buf_len, uint16_t timeout_ms) {

    byte packet_len;
    byte packet[MAX_PACKET_LEN];

    data_link_rx_result result;

    // Repeat this until we get something that's for me,
    // or until one of our things times out.
    while(true) {
        uart_transmit_formatted_message("Trying to receive a packet.\r\n");
        UART_WAIT_UNTIL_DONE();

        result = data_link_rx(packet, MAX_PACKET_LEN, timeout_ms);
        if (result == DATA_LINK_RX_ERROR) {
            uart_transmit_formatted_message("[WARNING] Error in network_rx\r\n");
            UART_WAIT_UNTIL_DONE();
            return NETWORK_RX_ERROR;
        }
        if (result == DATA_LINK_RX_TIMEOUT) {
            uart_transmit_formatted_message("[INFO] Timeout in network_rx\r\n");
            UART_WAIT_UNTIL_DONE();
            return NETWORK_RX_TIMEOUT;
        }

        LED_blink(LED_BLUE);

        uart_transmit_formatted_message("Received a packet: ");
        UART_WAIT_UNTIL_DONE();
        print_packet(packet);

        packet_len = packet[0];

        // Packet is for me. Copy data and return true.
        if (packet[1] == MY_NETWORK_ADDR) {
            for (byte i = 0; i < packet_len - PACKET_HEADER_LEN && i < buf_len; i++) {
                buffer[i] = packet[i + PACKET_HEADER_LEN];
            }
            return NETWORK_RX_SUCCESS;
        }

        // Packet is not for me. Forward it and try again.
        network_tx(&packet[PACKET_HEADER_LEN], packet_len - PACKET_HEADER_LEN, packet[1], packet[2]);
    }
}

// Transmit to the specified network address.
network_tx_result network_tx(byte* payload, byte payload_len, byte dest_network_addr, byte src_network_addr) {

    _delay_ms(NETWORK_DELAY_MS);

    data_link_tx_result result;
    byte packet_len = payload_len + PACKET_HEADER_LEN;
    byte packet[packet_len];

    packet[0] = packet_len;
    packet[1] = dest_network_addr;
    packet[2] = src_network_addr;
    for (byte i = 0; i < payload_len && i < MAX_PACKET_LEN - PACKET_HEADER_LEN; i++) {
        packet[i + PACKET_HEADER_LEN] = payload[i];
    }
    byte next_hop_addr = routing_table(dest_network_addr);

    LED_blink(LED_BLUE);
    uart_transmit_formatted_message("Transmitting a packet: ");
    UART_WAIT_UNTIL_DONE();
    print_packet(packet);

    result = data_link_tx(packet, packet_len, resolve_data_link_addr(next_hop_addr));

    if (result == DATA_LINK_TX_FAILURE) return NETWORK_TX_FAILURE;

    return NETWORK_TX_SUCCESS;
}

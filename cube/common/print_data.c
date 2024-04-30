#include "print_data.h"
#include "transport.h"
#include "uart.h"

void print_segment(byte* segment) {

    // four segment types: START_OF_MESSAGE, DATA, END_OF_MESSAGE, ACK
    int segtype = segment[4];

    switch(segtype) {

    // START_OF_MESSAGE segment:
    // segment[0] = length of segment = 6
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x07, START_OF_MESSAGE
    // segment[5-6] = total length of message

    case SEGID_START_OF_MESSAGE:
        uart_transmit_formatted_message("\t\tLength of segment:          %d\r\n", segment[0]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSequence number:            %d\r\n", segment[1]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tDestination port number:    %02x\r\n", segment[2]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSource port number:         %02x\r\n", segment[3]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSegment identifier:         %02x (START_OF_MESSAGE)\r\n", segment[4]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tTotal message length:       %d\r\n", ((segment[5] & 0xFF00) << 8) + ((segment[6] & 0x00FF) << 0));
        UART_WAIT_UNTIL_DONE();
        break;

    // DATA segment:
    // segment[0] = length of segment
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x0D, DATA
    // segment[5-6] = start address (starting memory address of this segment's data)
    // rest is payload

    case SEGID_DATA:
        uart_transmit_formatted_message("\t\tLength of segment:          %d\r\n", segment[0]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSequence number:            %d\r\n", segment[1]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tDestination port number:    %02x\r\n", segment[2]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSource port number:         %02x\r\n", segment[3]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSegment identifier:         %02x (DATA)\r\n", segment[4]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tStart address:              %02x\r\n", ((segment[5] & 0xFF00) << 8) + ((segment[6] & 0x00FF) << 0));
        UART_WAIT_UNTIL_DONE();
        break;

    // END_OF_MESSAGE segment:
    // segment[0] = length of segment = 5
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x09, END_OF_MESSAGE

    case SEGID_END_OF_MESSAGE:
        uart_transmit_formatted_message("\t\tLength of segment:          %d\r\n", segment[0]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSequence number:            %d\r\n", segment[1]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tDestination port number:    %02x\r\n", segment[2]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSource port number:         %02x\r\n", segment[3]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSegment identifier:         %02x (END_OF_MESSAGE)\r\n", segment[4]);
        UART_WAIT_UNTIL_DONE();
        break;

    // ACK segment:
    // segment[0] = length of segment = 5
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x0A, ACK

    case SEGID_ACK:
        uart_transmit_formatted_message("\t\tLength of segment:          %d\r\n", segment[0]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSequence number:            %d\r\n", segment[1]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tDestination port number:    %02x\r\n", segment[2]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSource port number:         %02x\r\n", segment[3]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSegment identifier:         %02x (ACK)\r\n", segment[4]);
        UART_WAIT_UNTIL_DONE();
        break;

    default:
        uart_transmit_formatted_message("\t\tLength of segment:          %d\r\n", segment[0]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSequence number:            %d\r\n", segment[1]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tDestination port number:    %02x\r\n", segment[2]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSource port number:         %02x\r\n", segment[3]);
        UART_WAIT_UNTIL_DONE();
        uart_transmit_formatted_message("\t\tSegment identifier:         %02x (INVALID)\r\n", segment[4]);
        UART_WAIT_UNTIL_DONE();
        break;
    }
}

void print_packet(byte* packet) {

    // packet[0] = length of packet
    // packet[1] = final destination network address
    // packet[2] = original source network address
    // rest is payload

    uart_transmit_formatted_message("\t========== Packet ==========\r\n");
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message("\tPacket length:    %d\r\n", packet[0]);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message("\tDestination addr: %02x\r\n", packet[1]);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message("\tSource addr:      %02x\r\n", packet[2]);
    UART_WAIT_UNTIL_DONE();
    uart_transmit_formatted_message("\tPayload:\r\n");
    UART_WAIT_UNTIL_DONE();
    print_segment(&packet[PACKET_HEADER_LEN]);
    uart_transmit_formatted_message("\t============================\r\n");
    UART_WAIT_UNTIL_DONE();

}
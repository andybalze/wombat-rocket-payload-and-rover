#include "print_data.h"
#include "transport.h"

void print_segment(byte* segment) {

    char payload[256];
    for (int i = 0; i < 256; i++) {
        payload[i] = '\0';
    }


    // four segment types: START_OF_MESSAGE, DATA, END_OF_MESSAGE, ACK
    int segtype = segment[4];

    switch(segtype) {

    // START_OF_MESSAGE segment:
    // segment[0] = length of segment = 6
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x07, START_OF_MESSAGE
    // segment[5] = total length of message

    case SEGID_START_OF_MESSAGE:
        printf("\t\t========== Segment (Start of Message) ===========\n");
        printf("\t\tLength of segment:          %d\n", segment[0]);
        printf("\t\tSequence number:            %d\n", segment[1]);
        printf("\t\tDestination port number:    %02x\n", segment[2]);
        printf("\t\tSource port number:         %02x\n", segment[3]);
        printf("\t\tSegment identifier:         %02x (START_OF_MESSAGE)\n", segment[4]);
        printf("\t\tTotal message length:       %d\n", segment[5]);
        printf("\t\t=================================================\n");
        break;

    // DATA segment:
    // segment[0] = length of segment
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x0D, DATA
    // segment[5] = start address (starting memory address of this segment's data)
    // rest is payload

    case SEGID_DATA:
        int payload_length = segment[0] - DATA_SEGMENT_HEADER_LEN;
        for (int i = 0; i < payload_length; i++) {
            payload[i] = segment[i + DATA_SEGMENT_HEADER_LEN];
        }
        printf("\t\t========== Segment (Data) ===========\n");
        printf("\t\tLength of segment:          %d\n", segment[0]);
        printf("\t\tSequence number:            %d\n", segment[1]);
        printf("\t\tDestination port number:    %02x\n", segment[2]);
        printf("\t\tSource port number:         %02x\n", segment[3]);
        printf("\t\tSegment identifier:         %02x (DATA)\n", segment[4]);
        printf("\t\tStart address:              %02x\n", segment[5]);
        printf("\t\tThe payload:                %s\n", payload);
        printf("\t\t=================================================\n");
        break;

    // END_OF_MESSAGE segment:
    // segment[0] = length of segment = 5
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x09, END_OF_MESSAGE

    case SEGID_END_OF_MESSAGE:
        printf("\t\t========== Segment (End of Message) ===========\n");
        printf("\t\tLength of segment:          %d\n", segment[0]);
        printf("\t\tSequence number:            %d\n", segment[1]);
        printf("\t\tDestination port number:    %02x\n", segment[2]);
        printf("\t\tSource port number:         %02x\n", segment[3]);
        printf("\t\tSegment identifier:         %02x (END_OF_MESSAGE)\n", segment[4]);
        printf("\t\t=================================================\n");
        break;

    // ACK segment:
    // segment[0] = length of segment = 5
    // segment[1] = sequence number
    // segment[2] = destination port number
    // segment[3] = source port number
    // segment[4] = segment identifier = 0x0A, ACK

    case SEGID_ACK:
        printf("\t\t========== Segment (Acknowledgement) ===========\n");
        printf("\t\tLength of segment:          %d\n", segment[0]);
        printf("\t\tSequence number:            %d\n", segment[1]);
        printf("\t\tDestination port number:    %02x\n", segment[2]);
        printf("\t\tSource port number:         %02x\n", segment[3]);
        printf("\t\tSegment identifier:         %02x (ACK)\n", segment[4]);
        printf("\t\t=================================================\n");
        break;

    default:
        printf("\t\t========== Segment (Invalid) ===========\n");
        printf("\t\tLength of segment:          %d\n", segment[0]);
        printf("\t\tSequence number:            %d\n", segment[1]);
        printf("\t\tDestination port number:    %02x\n", segment[2]);
        printf("\t\tSource port number:         %02x\n", segment[3]);
        printf("\t\tSegment identifier:         %02x (ACK)\n", segment[4]);
        printf("\t\t========================================\n");
        break;
    }
}

void print_packet(byte* packet) {

    // packet[0] = length of packet
    // packet[1] = final destination network address
    // packet[2] = original source network address
    // rest is payload

    printf("\t========== Packet ==========\n");
    printf("\tPacket length:    %d\n", packet[0]);
    printf("\tDestination addr: %02x\n", packet[1]);
    printf("\tSource addr:      %02x\n", packet[2]);
    printf("\tPayload:\n");
    print_segment(&packet[PACKET_HEADER_LEN]);
    printf("\t============================\n");

}
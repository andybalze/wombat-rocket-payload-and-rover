#ifndef _NETWORING_CONSTANTS_H
#define _NETWORING_CONSTANTS_H

#include <stdint.h>

typedef uint8_t byte;

#define MAX_FRAME_LEN (32)
#define FRAME_HEADER_LEN (1)

#define MAX_PACKET_LEN (31)
#define PACKET_HEADER_LEN (3)

#define MAX_SEGMENT_LEN (28) 
#define START_SEGMENT_HEADER_LEN (5)
#define DATA_SEGMENT_HEADER_LEN (5)
#define END_SEGMENT_HEADER_LEN (4)

#define MAX_MESSAGE_LEN (255)

#define MY_NETWORK_ADDR (0x0A) // later let's figure out how to move this guy to the makefile or something
#define MY_PORT (0x0A)

#endif
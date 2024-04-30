#ifndef _NETWORING_CONSTANTS_H
#define _NETWORING_CONSTANTS_H

#include <stdint.h>

typedef uint8_t byte;

#define MAX_FRAME_LEN (32)
#define FRAME_HEADER_LEN (1)

#define MAX_PACKET_LEN (31)
#define PACKET_HEADER_LEN (3)

#define MAX_SEGMENT_LEN (28) 
#define START_SEGMENT_HEADER_LEN (8)
#define DATA_SEGMENT_HEADER_LEN (8)
#define END_SEGMENT_HEADER_LEN (6)
#define ACK_SEGMENT_HEARDER_LEN (6)

#define MAX_MESSAGE_LEN (256)
#define MESSAGE_HEADER_LEN (1)

#endif
#ifndef _NETWORING_CONSTANTS_H
#define _NETWORING_CONSTANTS_H

// to-do: add MAX_FRAME_LEN
#define MAX_PACKET_LEN (32)
#define MAX_SEGMENT_LEN (MAX_PACKET_LEN - 3) // data segment header = 3 bytes
#define MAX_MESSAGE_LEN (256)

#endif
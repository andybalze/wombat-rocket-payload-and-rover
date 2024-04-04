#ifndef _TRANSPORT_H
#define _TRANSPORT_H

// This function receives a message and stores it in the buffer.
// This is a blocking function that will only return when
// it has received data.
// The return value is the length of the message.
int transport_rx(char* buffer, int buf_len);

void transport_tx(char* message, int message_len, int dest_port);

#endif

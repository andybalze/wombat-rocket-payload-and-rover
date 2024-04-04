#ifndef _DATA_LINK_H
#define _DATA_LINK_H

int data_link_push_to_fifo(char* buffer, int buf_len);

int data_link_rx(char* buffer, int buf_len);

void data_link_tx(char* payload, int payload_len, int addr);

#endif
#ifndef _NETWORK_H
#define _NETWORK_H

// This function blocks and waits until it receives a packet
// destined for us. It might even forward packets while waiting.
// How fun!
int network_rx(char* buffer, int buf_len);

int network_tx(char* buffer, int buf_len);

#endif
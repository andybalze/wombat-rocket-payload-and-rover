#ifndef _NETWORK_H
#define _NETWORK_H

// This function blocks and waits until it receives a packet
// destined for us. It might even forward packets while waiting.
// How fun!
int network_rx(char* buffer, int buf_len);

void network_tx(char* payload, int payload_len, int dest_network_addr, int src_network_addr);

#endif
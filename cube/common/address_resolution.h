#ifndef _ADDRESS_RESOLUTION_H
#define _ADDRESS_RESOLUTION_H

/*
    In a protocol stack, you will have a different kind of address for every
    layer. This is true for our protocol stack: our data link address is 4
    bytes long, but our network address is only 1 byte long.

    We need a way to connect these addresses. That is the purpose of these
    functions. If you're curious, search "Address Resolution Protocol" to
    see how the TCP/IP stack does it. It's a little bizarre.
*/

int resolve_data_link_addr(int network_addr);
int resolve_network_addr(int port);

#endif

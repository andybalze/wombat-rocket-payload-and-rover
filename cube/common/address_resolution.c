#include "address_resolution.h"

int resolve_data_link_addr(int network_addr) {
    // We have, like, four devices total. Let's not make this hard on ourselves.
    int data_link_addr;
    switch (network_addr) {
    case 0x0A:
        data_link_addr = 0x0A0A0A0A;
        break;
    case 0x0B:
        data_link_addr = 0x0B0B0B0B;
        break;
    case 0x0C:
        data_link_addr = 0x0C0C0C0C;
        break;
    case 0x0D:
        data_link_addr = 0x0D0D0D0D;
        break;
    }
    return data_link_addr;
}

int resolve_network_addr(int port) {
    // They aren't so different after all!
    // This function exists in case you want to make the transport layer
    // more elaborate.
    return port;
}

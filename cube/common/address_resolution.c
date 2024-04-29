#include "address_resolution.h"

uint32_t resolve_data_link_addr(byte network_addr) {
    // We have, like, four devices total. Let's not make this hard on ourselves.
    uint32_t data_link_addr;
    switch (network_addr) {
    case 0x3A:
        data_link_addr = 0x3A3A3A3A;
        break;
    case 0x3B:
        data_link_addr = 0x3B3B3B3B;
        break;
    case 0x3C:
        data_link_addr = 0x3C3C3C3C;
        break;
    case 0x3F:
        data_link_addr = 0x3F3F3F3F;
        break;
    }
    return data_link_addr;
}

byte resolve_network_addr(byte port) {
    // This function exists in case you want to make the transport layer
    // more elaborate. But for now...
    return port;
}

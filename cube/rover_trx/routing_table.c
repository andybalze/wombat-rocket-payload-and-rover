#include "routing_table.h"

byte routing_table(byte final_addr) {
    byte next_hop_addr;
    switch(final_addr) {
    case 0x3A:
        next_hop_addr = 0x3C;
        break;
    case 0x3B:
        next_hop_addr = 0x3C;
        break;
    case 0x3C:
        next_hop_addr = 0x3C;
        break;
    case 0x3F:
        next_hop_addr = 0x3F;
        break;
    }
    return next_hop_addr;
}

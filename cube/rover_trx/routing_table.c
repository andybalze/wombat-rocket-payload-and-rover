#include "routing_table.h"

byte routing_table(byte final_addr) {
    byte next_hop_addr;
    switch(final_addr) {
    case 0x0A:
        next_hop_addr = 0x0C;
        break;
    case 0x0B:
        next_hop_addr = 0x0C;
        break;
    case 0x0C:
        next_hop_addr = 0x0C;
        break;
    case 0x0F:
        next_hop_addr = 0x0F;
        break;
    }
    return next_hop_addr;
}

#include "log.h"
#include "uart.h"

#include <avr/eeprom.h>

/*
    EEPROM Layout

    eeprom[0..1]        = message count; used to determine next message slot
    eeprom[2..63]       = <reserved>

    eeprom[64]          = source address of message in slot 0
    eeprom[65..66]      = length of message in slot 0
    eeprom[67..127]     = <reserved>

    eeprom[128]         = source address of message in slot 1
    eeprom[129..130]    = length of message in slot 1
    eeprom[131..191]    = <reserved>

    eeprom[192]         = source address of message in slot 2
    eeprom[193..194]    = length of message in slot 2
    eeprom[195..255]    = <reserved>

    eeprom[256..511]    = message slot 0
    eeprom[512..767]    = message slot 1
    eeprom[768..1023]   = message slot 2
*/







// ---------------- Private functions -------------

uint16_t get_message_count() {
    return (eeprom_read_byte((uint8_t*)0) << 8) + (eeprom_read_byte((uint8_t*)1) << 0);
}

void set_message_count(uint16_t count) {
    uint8_t top = (count & 0xFF00) >> 8;
    uint8_t bot = (count & 0x00FF) >> 0;
    eeprom_update_byte((uint8_t*)0, top);
    eeprom_update_byte((uint8_t*)1, bot);
    return;
}

uint8_t get_message_source(uint8_t slot) {
    size_t addr = 64*(slot+1);
    return eeprom_read_byte((uint8_t*)addr);
}

void set_message_source(uint8_t source, uint8_t slot) {
    size_t addr = 64*(slot+1);
    eeprom_update_byte((uint8_t*)addr, source);
    return;
}

uint16_t get_message_len(uint8_t slot) {
    size_t top_addr = 64*(slot+1) + 1;
    size_t bot_addr = top_addr + 1;
    return (eeprom_read_byte((uint8_t*)top_addr) << 8) + (eeprom_read_byte((uint8_t*)bot_addr) << 0);
}

void set_message_len(uint16_t len, uint8_t slot) {
    size_t top_addr = 64*(slot+1) + 1;
    size_t bot_addr = top_addr + 1;
    uint8_t top = (len & 0xFF00) >> 8;
    uint8_t bot = (len & 0x00FF) >> 0;
    eeprom_update_byte((uint8_t*)top_addr, top);
    eeprom_update_byte((uint8_t*)bot_addr, bot);
    return;
}

void get_message(byte* buffer, uint16_t buf_len, uint8_t slot) {
    size_t base_addr = 256*(slot+1);
    eeprom_read_block(buffer, (void*)base_addr, buf_len); // void pointer ???? not on my christian minecraft server
    return;
}

void set_message(byte* message, uint16_t message_len, uint8_t slot) {
    size_t base_addr = 256*(slot+1);
    eeprom_update_block(message, (void*)base_addr, message_len);
    return;
}







// ---------------- Public functions -------------

void log_message(byte* message, uint16_t message_len, uint8_t message_source) {

    // What is our message count?
    uint16_t message_count = get_message_count();

    // Get what the next available slot is.
    uint8_t next_slot = message_count % 3;

    // Let's log the info.
    set_message_count(message_count + 1);
    set_message_source(message_source, next_slot);
    set_message_len(message_len, next_slot);
    set_message(message, message_len, next_slot);

    return;
}

void print_log() {

    char message_buf[256];

    uart_transmit_formatted_message("::: Log of Messages :::\r\n");
    UART_WAIT_UNTIL_DONE();

    // What is our message count?
    uint16_t message_count = get_message_count();
    uart_transmit_formatted_message("This cube has received %d messages.\r\n\r\n", message_count);
    UART_WAIT_UNTIL_DONE();


    // Get what the next available slot is.
    uint8_t slot = message_count % 3;


    // Print the latest messages in the log.
    for (int i = 0; i < 3 && message_count > i; i++) {
        slot = (slot == 0 ? 2 : slot - 1);

        uart_transmit_formatted_message("===== Logged message from %02x =====\r\n", get_message_source(slot));
        UART_WAIT_UNTIL_DONE();

        get_message(message_buf, 256, slot);
        message_buf[255] = '\0'; // force null termination just in case
        uart_transmit_formatted_message(message_buf);
        UART_WAIT_UNTIL_DONE();

        uart_transmit_formatted_message("====================================\r\n\r\n");
        UART_WAIT_UNTIL_DONE();
    }

    return;
}

void erase_log() {
    for (uint16_t i = 0; i < 1023; i++) {
        eeprom_update_byte((uint8_t*)i, 0);
    }
    return;
}

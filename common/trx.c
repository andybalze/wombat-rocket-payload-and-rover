////////////////////////////////////////////////////////////////////////////////
//
// TRX
//
// Provides functions for interfacing with the nRF24L01+ wireless transceiver.
// These functions use the ATMega's SPI. While using these functions, the SPI
// may not be used for any other purpose. The settings in spi.h are assumemd to
// be correct for driving the transceiver.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>

#include "trx.h"

#include "spi.h"
#include "uart.h"

/////////////////// Private Defines ////////////////////////////////////////////

#define TRX_TRANSACTION_MAX_LENGTH  (33)
// 1 for the instruction, up to 32 for the data.

// Transceiver instruction definitions.

// No-op used for reading the status register.
#define TRX_NOOP_INSTRUCTION                  (0xFF)      
#define TRX_NOOP_TRANSACTION_LENGTH           (1)

// Reads the value of a configuration register.
#define TRX_READ_REGISTER_INSTRUCTION         (0x00)
#define TRX_READ_REGISTER_ADDRESS_MASK        (0x1F)

// Writes the value of a configuration register.
#define TRX_WRITE_REGISTER_INSTRUCTION        (0x20)
#define TRX_WRITE_REGISTER_ADDRESS_MASK       (0x1F)

// Register Addresses

// Configuration register
#define TRX_REGISTER_ADDRESS_CONFIG     (0x00)

// Auto Acknowledgement Enable
#define TRX_REGISTER_ADDRESS_EN_AA      (0x01)

// Enable data pipes
#define TRX_REGISTER_ADDRESS_EN_RXADDR  (0x02)

// Address Widths
#define TRX_REGISTER_ADDRESS_SETUP_AW   (0x03)

// Register settings

// This design uses only four-byte addresses.
#define TRX_SETUP_AW (0x02) // 10 -> 4 bytes

/////////////////// Static Variable Definitions ////////////////////////////////

trx_status_buffer_t trx_status_buffer;

const spi_message_element_t message_setup_aw_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_SETUP_AW & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};
const int message_setup_aw_read_length = 2;

const spi_message_element_t message_setup_aw_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_SETUP_AW & TRX_READ_REGISTER_ADDRESS_MASK),
  TRX_SETUP_AW
};
const int message_setup_aw_write_length = 2;

/////////////////// Private Function Prototypes ////////////////////////////////

void update_status_buffer (
  const spi_message_element_t *received_message,
  int received_message_length
);

void uart_transmit_register_value(
  const spi_message_element_t *received_message,
  int received_message_length
);

/////////////////// Public Function Bodies /////////////////////////////////////

// Initializes the TRX, including initializing the SPI and any other peripherals
// required.
void trx_initialize() {

  // Sets the CE pin as an output.
  TRX_CE_DDR |= _BV(TRX_CE_INDEX);
  
  // TODO: Set up interrupt on TRX_IRQ pin.

  spi_initialize();

  spi_begin_transaction(
    message_setup_aw_read,
    message_setup_aw_read_length,
    uart_transmit_register_value
  );
  SPI_WAIT_UNTIL_DONE();
  UART_WAIT_UNTIL_DONE();

  spi_begin_transaction(
    message_setup_aw_write,
    message_setup_aw_write_length,
    NULL
  );
  SPI_WAIT_UNTIL_DONE();

  spi_begin_transaction(
    message_setup_aw_read,
    message_setup_aw_read_length,
    uart_transmit_register_value
  );
  SPI_WAIT_UNTIL_DONE();
  UART_WAIT_UNTIL_DONE();

}

void trx_transmit_payload(
  trx_address_t address,
  trx_payload_element_t *payload,
  int payload_length
) {
  // Not currently implemented.
  return;
}

// Gets the value currently in the status buffer. This is equivalent to what was
// in the transceiver's status register at the beginning of the last SPI
// transaction.
trx_status_buffer_t trx_get_status() {
  return trx_status_buffer;
}

/////////////////// Private function bodies ////////////////////////////////////

void update_status_buffer(
  const spi_message_element_t *received_message,
  int received_message_length
) {
  trx_status_buffer = received_message[0];
}

void uart_transmit_register_value(
  const spi_message_element_t *received_message,
  int received_message_length
) {
  uart_transmit_formatted_message(
    "Register value: %02x\n\r",
    received_message[1]
  );
}

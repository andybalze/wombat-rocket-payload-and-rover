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

/////////////////// Private Defines ////////////////////////////////////////////

// Transceiver instruction definitions.

// No-op used for reading the status register.
#define TRX_NOOP_INSTRUCTION        (0xFF)      
#define TRX_NOOP_TRANSACTION_LENGTH (1)

/////////////////// Static Variable Definitions ////////////////////////////////

trx_status_buffer_t trx_status_buffer;

/////////////////// Private Function Prototypes ////////////////////////////////

void update_status_buffer (
  const spi_message_element_t *received_message,
  int received_message_length
);

/////////////////// Public Function Bodies /////////////////////////////////////

// Initializes the TRX, including initializing the SPI and any other peripherals
// required.
void trx_initialize() {

  // Sets up GPIO pins used to drive transceiver.

  // Sets the CE pin as an output.
  TRX_CE_DDR |= _BV(TRX_CE_INDEX);
  
  // TODO: Set up interrupt on TRX_IRQ pin.

  spi_initialize();

  // Send a no-op over the SPI in order to update the trx status buffer.
  spi_message_element_t instruction;
  instruction = TRX_NOOP_INSTRUCTION;
  spi_begin_transaction(
    &instruction,
    1,
    update_status_buffer
  );

  // Wait until the SPI message has finished sending.
  while ((SPCR & _BV(SPIE)) != 0);

}

void trx_transmit_payload(
  trx_address_t address,
  trx_payload_element_t payload,
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

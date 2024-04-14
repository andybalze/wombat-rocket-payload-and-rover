/*

        WARNING

        This is a SIMULATION FILE.
        Do not compile this code for hardware.

*/


#ifndef _TRX_H
#define _TRX_H

#include <stdint.h>

/////////////////// TRX Settings ///////////////////////////////////////////////

// The length of payloads transmitted and received by this transceiver.
#define TRX_PAYLOAD_LENGTH (32)

// Addresses are 32 bits wide, or 4 bytes.
typedef uint32_t trx_address_t;

// Payloads are transmitted one byte at a time.
typedef uint8_t trx_payload_element_t;

// The type of the transceiver's status register.
typedef uint8_t trx_status_buffer_t;

// Whether a given transmission succeeded or failed.
typedef uint8_t trx_transmission_outcome_t;
#define TRX_TRANSMISSION_FAILURE (0)
#define TRX_TRANSMISSION_SUCCESS (1)

/////////////////// Public function prototypes /////////////////////////////////

// Initializes the TRX, including initializing the SPI and any other peripherals
// required.
void trx_initialize(
  trx_address_t rx_address
);

// Transmits a payload to the given address.
trx_transmission_outcome_t trx_transmit_payload(
  trx_address_t address,
  trx_payload_element_t *payload,
  int payload_length
);

// Receives a payload using polling.
int trx_receive_payload(
  trx_payload_element_t *payload_buffer
);

// Gets the value currently in the status buffer. This is equivalent to what was
// in the transceiver's status register at the beginning of the last SPI
// transaction.
trx_status_buffer_t trx_get_status();

#endif
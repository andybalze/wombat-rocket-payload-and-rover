#ifndef _TRX_H
#define _TRX_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

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

/////////////////// TRX Settings ///////////////////////////////////////////////

// The length of payloads transmitted and received by this transceiver.
#define TRX_PAYLOAD_LENGTH (32)

// Payloads shorter than TRX_PAYLOAD_LENGTH are padded with this before being
// transmitted.
#define TRX_PAYLOAD_PADDING (0x00)

// The ports and pins used to drive the chip-enable pin of the transceiver.
#define TRX_CE_DDR    DDRB
#define TRX_CE_PORT   PORTB
#define TRX_CE_PIN    PINB
#define TRX_CE_INDEX  (1)

// The ports and pins used for the transceiver's IRQ interrupt.
#define TRX_IRQ_DDR       DDRD
#define TRX_IRQ_PORT      PORTD
#define TRX_IRQ_PIN       PIND
#define TRX_IRQ_INDEX     (2)
#define TRX_IRQ_INT       INT0
#define TRX_IRQ_INT_vect  INT0_vect

// Passing this to trx_receive_payload will cause the function to wait for a
// reception indefinitely.
#define TRX_TIMEOUT_INDEFINITE (1501)

/////////////////// TRX Macros /////////////////////////////////////////////////

// Whether the transceiver has requested an interrupt
#define TRX_IRQ ((TRX_IRQ_PIN & _BV(TRX_IRQ_INDEX)) == 0)

// Waits until the TRX_IRQ pin goes low.
#define TRX_WAIT_FOR_IRQ() while(!TRX_IRQ)

/////////////////// TRX type definitions ///////////////////////////////////////

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

// Whether a given attempt to receive a transmission succeeded or failed.
typedef uint8_t trx_reception_outcome_t;
#define TRX_RECEPTION_FAILURE (0)
#define TRX_RECEPTION_SUCCESS (1) 

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
trx_reception_outcome_t trx_receive_payload(
  trx_payload_element_t *payload_buffer,
  timer_delay_ms_t timeout_ms
);

// Gets the value currently in the status buffer. This is equivalent to what was
// in the transceiver's status register at the beginning of the last SPI
// transaction.
trx_status_buffer_t trx_get_status();

#endif
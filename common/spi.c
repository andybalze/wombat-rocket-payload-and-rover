////////////////////////////////////////////////////////////////////////////////
//
// SPI
//
// Provides functions that allow the use of the ATMega's SPI peripheral.
// The ATMega is always set to be a SPI Master, but details of its operation can
// be configured via the provided macros. Only a single peripheral may be
// connected.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>

#include "spi.h"

//////////////////// Private Defines ///////////////////////////////////////////

#define SPI_DORD_MSB_FIRST (0)
#define SPI_DORD_LSB_FIRST ( _BV(DORD) )
#if SPI_DATA_ORDER == SPI_DATA_ORDER_MSB_FIRST
  #define SPI_DORD SPI_DORD_MSB_FIRST
#elif SPI_DATA_ORDER == SPI_DATA_ORDER_MSB_FIRST
  #define SPI_DORD SPI_DORD_LSB_FIRST
#else
  #define SPI_DATA_ORDER == SPI_DATA_ORDER_MSB_FIRST
#endif

#define SPI_CPOL_IDLE_LOW   (0)
#define SPI_CPOL_IDLE_HIGH  ( _BV(CPOL) )
#if SPI_CLOCK_POLARITY == SPI_CLOCK_POLARITY_IDLE_LOW
  #define SPI_CPOL SPI_CPOL_IDLE_LOW
#elif SPI_CLOCK_POLARITY == SPI_CLOCK_POLARITY_IDLE_HIGH
  #define SPI_CPOL SPI_CPOL_IDLE_HIGH
#else
  #define SPI_cpOL SPI_CPOL_IDLE_LOW
#endif

#define SPI_CPHA_SAMPLE_LEADING   (0)
#define SPI_CPHA_SAMPLE_TRAILING  ( _BV(CPHA) )
#if SPI_CLOCK_PHASE == SPI_CLOCK_PHASE_SAMPLE_LEADING
  #define SPI_CPHA SPI_CPHA_SAMPLE_LEADING
#elif SPI_CLOCK_PHASE == SPI_CLOCK_PHASE_SAMPLE_TRAILING
  #define SPI_CPHA SPI_CPHA_SAMPLE_TRAILING
#else
  #define SPI_CPHA SPI_CPHA_SAMPLE_LEADING
#endif

#define SPI_SPR_4     (0)
#define SPI_SPR_16    ( _BV(SPR0)             )
#define SPI_SPR_64    (             _BV(SPR1) )
#define SPI_SPR_128   ( _BV(SPR0) | _BV(SPR1) )
#define SPI_SPR_MASK  ( _BV(SPR1) | _BV(SPR0) )
#if SPI_CLOCK_IDEAL_PRESCALAR <= 4
  #define SPI_SPR (SPI_SPR_4 & SPI_SPR_MASK)
#elif SPI_CLOCK_IDEAL_PRESCALAR <= 16
  #define SPI_SPR (SPI_SPR_16 & SPI_SPR_MASK)
#elif SPI_CLOCK_IDEAL_PRESCALAR <= 64
  #define SPI_SPR (SPI_SPR_64 & SPI_SPR_MASK)
#elif SPI_CLOCK_IDEAL_PRESCALAR <= 128
  #define SPI_SPR (SPI_SPR_128 & SPI_SPR_MASK)
#else
  #define SPI_SPR (SPI_SPR_128 & SPI_SPR_MASK)
#endif

//////////////////// Static Variable Definitions ///////////////////////////////

// The message currently being received.
static spi_message_element_t receive_message_buffer[SPI_TRANSACTION_MAX_LENGTH];

// The message currently being transmitted.
static spi_message_element_t transmit_message_buffer[SPI_TRANSACTION_MAX_LENGTH];

// The index of the transaction bit that is currently being transmitted and received.
static int transaction_index;

// The length of the current transaction.
static int current_transaction_length;

// The callback that will be executed once the current transaction finishes.
static void (*current_transaction_complete_callback)(
  const spi_message_element_t *received_message,
  int received_message_length
);

//////////////////// Public Function Bodies ////////////////////////////////////

// Initialize the SPI, including configuring the appropriate pins.
void spi_initialize(void) {

  // Configure the SPI pins appropriately.
  DDRB |= (
    0
    | _BV(DDB5)   // Sets the SCK pin as an output.
    | _BV(DDB3)   // Sets the MOSI pin as an output.
    | _BV(DDB2)   // Sets the SS pin as an output.
  );
  // Other pins (MISO) are inputs.
  
  // SPCR is the only SPI register that needs to be configured.  
  SPCR = (
    0
    // | _BV(SPIE)   // Enables SPI interrupt.
    | _BV(SPE)    // Enables SPI.
    | SPI_DORD    // Configures data direction.
    | _BV(MSTR)   // Configures the SPI as a master.
    | SPI_CPOL    // Configures the SPI clock polarity.
    | SPI_CPHA    // Configures the SPI clock phase.
    | SPI_SPR     // Configures the SPI prescaler.
  );

}

// Transmits a message over the SPI.
void spi_begin_transaction(
  const spi_message_element_t *transmit_message,
  int transaction_length,
  void (*transaction_complete_callback)(
    const spi_message_element_t *received_message,
    int received_message_length
  )
) {

  // If a SPI transaction is already underway, another one can be started.
  int transaction_underway;
  transaction_underway = SPCR & _BV(SPIE);
  if (transaction_underway != 0) {
    return;
  }

  // If the transaction length is larger than the maximum allowed, the
  // transaction cannot be started.
  if (transaction_length > SPI_TRANSACTION_MAX_LENGTH) {
    return;
  }
  
  // Copies the message to transmit into the buffer and clears the receiving 
  // buffer.
  int i;
  for  (i = 0; i < SPI_TRANSACTION_MAX_LENGTH; i++) {
    if (i < transaction_length) {
      transmit_message_buffer[i] = transmit_message[i];
    } else {
      transmit_message_buffer[i] = 0;
    }
    receive_message_buffer[i] = 0;
  }

  // Start at the beginning of the transaction.
  transaction_index = 0;
  SPDR = transmit_message_buffer[0];

  // The transaction has successfully been started.
  SPCR |= _BV(SPIE);  // Enable the SPI interrupt.
  current_transaction_length = transaction_length;
  current_transaction_complete_callback = transaction_complete_callback;

}

ISR(SPI_STC_vect) {

  // Store the received data.
  receive_message_buffer[transaction_index] = SPDR;

  // Check if the transaction has been completed.
  if (transaction_index == current_transaction_length - 1) {

    // Stop the transaction and call the callback.
    SPCR &= ~_BV(SPIE);
    current_transaction_complete_callback(
      receive_message_buffer, 
      current_transaction_length
    );

  } else {

    // Transmit the next character
    transaction_index = transaction_index + 1;
    SPDR = transmit_message_buffer[transaction_index];

  }

}
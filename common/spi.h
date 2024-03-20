#ifndef _SPI_H
#define _SPI_H

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

/////////////////// SPI Settings ///////////////////////////////////////////////

// Determines the SPI data order.
#define SPI_DATA_ORDER_MSB_FIRST  0
#define SPI_DATA_ORDER_LSB_FIRST  1
#define SPI_DATA_ORDER  SPI_DATA_ORDER_MSB_FIRST

// Configures the clock polarity
#define SPI_CLOCK_POLARITY_IDLE_LOW   0
#define SPI_CLOCK_POLARITY_IDLE_HIGH  1
#define SPI_CLOCK_POLARITY  SPI_CLOCK_POLARITY_IDLE_LOW

// Determines whether the SPI will sample the MISO line on the leading or 
// trailing edge of the SPI clock. The MOSI line will be shifted out on the
// opposite clock edge.
#define SPI_CLOCK_PHASE_SAMPLE_LEADING  0
#define SPI_CLOCK_PHASE_SAMPLE_TRIALING 1
#define SPI_CLOCK_PHASE   SPI_CLOCK_PHASE_SAMPLE_LEADING

// The "Ideal" prescaler that the SPI clock should have, relative to the 
// CPU clock. The true SPI prescaler might not have this exact value, due to
// limitations 
#define SPI_CLOCK_IDEAL_PRESCALER 128;

// The frequency of the SPI clock in MHz. The true frequency of the SPI clock
// may not be exactly this.
#define SPI_FREQUENCY_MHZ (1)

#define SPI_TRANSACTION_MAX_LENGTH (64)

/////////////////// SPI Type Definitions ///////////////////////////////////////

typedef char spi_message_element_t;

/////////////////// Public Function Prototypes /////////////////////////////////

// Initialize the SPI, including configuring the appropriate pins.
void spi_initialize(void);

// Transmits a message over the SPI.
void spi_begin_transaction(
  const spi_message_element_t *transmit_message,
  int transaction_length,
  void (*transaction_complete_callback)(
    const spi_message_element_t *received_message,
    int received_message_length
  )
);

#endif
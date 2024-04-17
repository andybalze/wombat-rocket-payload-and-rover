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

#include <stdarg.h>

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

/////////////////// SPI macros /////////////////////////////////////////////////

// Waits until the current SPI transaction has concluded.
#define SPI_WAIT_UNTIL_DONE() while ((SPCR & _BV(SPIE)) != 0)

/////////////////// SPI Type Definitions ///////////////////////////////////////

typedef unsigned char spi_message_element_t;

/////////////////// Public Function Prototypes /////////////////////////////////

// Initialize the SPI, including configuring the appropriate pins.
void spi_initialize(void);

// Transmits a message over the SPI. The response of the slave is placed in the
// buffer passed in as the first argument, beginning with given beginning index.
// Any elements before that index are discarded. The message to transmit is 
// composed of a number of "sections." Each section consists of a buffer, 
// containing the data to transmit, and an integer, respresenting the number of 
// bytes in the section. For example,
//
// spi_execute_transaction(response, 1, 2, section1, section1_length, section2, section2_length);
//
// The third argument gives the number of sections that compose the message. If
// a section is given as "NULL", that section is filled with 0x00.
void spi_execute_transaction(
  spi_message_element_t *response,
  int response_beginning_index, 
  int section_count,
  ...
);

#endif
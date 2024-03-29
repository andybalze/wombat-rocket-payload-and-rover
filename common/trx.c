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

// Setup of Automatic Retransmission
#define TRX_REGISTER_ADDRESS_SETUP_RETR (0x04)

// RF Channel
#define TRX_REGISTER_ADDRESS_RF_CH      (0x05)

// Rf Setup Register
#define TRX_REGISTER_ADDRESS_RF_SETUP   (0x06)

// Status register
#define TRX_REGISTER_ADDRESS_STATUS     (0x07)

// Receive address for Data Pipe 0.
#define TRX_REGISTER_ADDRESS_RX_ADDR_P0 (0x0A)

// Transmit address.
#define TRX_REGISTER_ADDRESS_TX_ADDR    (0x10)

// Enable dynaic payload length.
#define TRX_REGISTER_ADDRESS_DYNPD      (0x1C)

// Feature Register
#define TRX_REGISTER_ADDRESS_FEATURE    (0x1D)

// Registers and fields

// Configuration register fields
#define MASK_RX_DR  (6)
#define MASK_DS     (5)
#define MASK_MAX_RT (4)
#define EN_CRC      (3)
#define CRCO        (2)
#define PWR_UP      (1)
#define PRIM_RX     (0)

// Only Pipe 0 is used.
#define TRX_EN_AA (0x01)

// Only Pipe 0 is used.
#define TRX_EN_RXADDR (0x01)

// This design uses only four-byte addresses.
#define TRX_SETUP_AW  (0x02) // 10 -> 4 bytes

// Maximum retransmission delay and tries.
#define TRX_SETUP_RETR (0xFF)

// Leaves the frequency channel at default.
#define TRX_RF_CH (0x02)

// No continuous carrier, 250kbps, no PLL lock, 0dBm output power.
#define TRX_RF_SETUP (0x26) // binary 0010 0110

// Status register
#define RX_DR     (6)
#define TX_DS     (5)
#define MAX_RT    (4)
#define RX_P_NO2  (3)
#define RX_P_NO1  (2)
#define RX_P_NO0  (1)
#define RX_P_NO   RX_P_NO2
#define TX_FULL   (0)

// Transmitter observation register
// Not used for this project.

// Received power detector
// Not used for this project.

// RX address is determined in trx.h.

// TX address is determined during code body.

// Number of bytes is data pipes is not relevant for this project.

// FIFO status register are not relevant to this project.

// Dynamic payload length is not used.
#define TRX_DYNPD (0x00)

// Dynamic payload length not used, no payload with acknowledgement, and no
// payloads with no acknowledgement.
#define TRX_FEATURE (0x00)

/////////////////// Static Variable Definitions ////////////////////////////////

trx_status_buffer_t trx_status_buffer;

const spi_message_element_t message_en_aa_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_EN_AA & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};
const spi_message_element_t message_en_aa_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_EN_AA & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_EN_AA
};

const spi_message_element_t message_en_rxaddr_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_EN_AA & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};
const spi_message_element_t message_en_rxaddr_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_EN_RXADDR & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_EN_RXADDR
};

const spi_message_element_t message_setup_aw_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_SETUP_AW & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};
const spi_message_element_t message_setup_aw_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_SETUP_AW & TRX_READ_REGISTER_ADDRESS_MASK),
  TRX_SETUP_AW
};

const spi_message_element_t message_setup_retr_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_SETUP_RETR & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};
const spi_message_element_t message_setup_retr_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_SETUP_RETR & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_SETUP_RETR
};

const spi_message_element_t message_rf_ch_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_RF_CH & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};
const spi_message_element_t message_rf_ch_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_RF_CH & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_RF_CH
};

const spi_message_element_t message_rf_setup_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_RF_SETUP & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_RF_SETUP
};
const spi_message_element_t message_rf_setup_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_RF_SETUP & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};

const spi_message_element_t message_dynpd_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_DYNPD & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_DYNPD
};
const spi_message_element_t message_dynpd_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_DYNPD & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};

const spi_message_element_t message_feature_write[] = {
  TRX_WRITE_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_FEATURE & TRX_WRITE_REGISTER_ADDRESS_MASK),
  TRX_FEATURE
};
const spi_message_element_t message_feature_read[] = {
  TRX_READ_REGISTER_INSTRUCTION | (TRX_REGISTER_ADDRESS_FEATURE & TRX_READ_REGISTER_ADDRESS_MASK),
  0
};

const int message_register_write_length = 2;
const int message_register_read_length  = 2;

/////////////////// Private Function Prototypes ////////////////////////////////

void update_status_buffer (
  const spi_message_element_t *received_message,
  int received_message_length
);

void uart_transmit_register_value(
  const spi_message_element_t *received_message,
  int received_message_length
);

void read_write_check_register(
  spi_message_element_t address,
  spi_message_element_t value
);

/////////////////// Public Function Bodies /////////////////////////////////////

// Initializes the TRX, including initializing the SPI and any other peripherals
// required.
void trx_initialize() {

  // Sets the CE pin as an output.
  TRX_CE_DDR |= _BV(TRX_CE_INDEX);
  
  // TODO: Set up interrupt on TRX_IRQ pin.

  spi_initialize();

  read_write_check_register(
    TRX_REGISTER_ADDRESS_EN_AA,
    TRX_EN_AA
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_EN_RXADDR,
    TRX_EN_RXADDR
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_SETUP_AW,
    TRX_SETUP_AW
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_SETUP_RETR,
    TRX_SETUP_RETR
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_RF_CH,
    TRX_RF_CH
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_RF_SETUP,
    TRX_RF_SETUP
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_DYNPD,
    TRX_DYNPD
  );

  read_write_check_register(
    TRX_REGISTER_ADDRESS_FEATURE,
    TRX_FEATURE
  );

  uart_transmit_formatted_message("SPI initialization complete!\n\r");
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
    "%02x",
    received_message[1]
  );
}

void read_write_check_register(
  spi_message_element_t address,
  spi_message_element_t value
) {

  spi_message_element_t message_read[] = {
    TRX_READ_REGISTER_INSTRUCTION | (address | TRX_READ_REGISTER_ADDRESS_MASK),
    0
  };
  spi_message_element_t message_write[] = {
    TRX_WRITE_REGISTER_INSTRUCTION | (address | TRX_WRITE_REGISTER_ADDRESS_MASK),
    value
  };

  uart_transmit_formatted_message(
    "Reading register %02x:\t", 
    address
  );
  UART_WAIT_UNTIL_DONE();
  spi_begin_transaction(
    message_read,
    message_register_read_length,
    uart_transmit_register_value
  );
  SPI_WAIT_UNTIL_DONE();
  UART_WAIT_UNTIL_DONE();
  uart_transmit_formatted_message("\n\r");
  UART_WAIT_UNTIL_DONE();

  uart_transmit_formatted_message(
    "Writing register %02x:\t%02x\n\r", 
    address, 
    value
  );
  UART_WAIT_UNTIL_DONE();
  spi_begin_transaction(
    message_write,
    message_register_write_length,
    NULL
  );
  SPI_WAIT_UNTIL_DONE();

  uart_transmit_formatted_message(
    "Reading register %02x:\t", 
    address
  );
  UART_WAIT_UNTIL_DONE();
  spi_begin_transaction(
    message_read,
    message_register_read_length,
    uart_transmit_register_value
  );
  SPI_WAIT_UNTIL_DONE();
  UART_WAIT_UNTIL_DONE();
  uart_transmit_formatted_message("\n\r\n\r");
  UART_WAIT_UNTIL_DONE();

}

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

#define F_CPU 1000000
#include <util/delay.h>

/////////////////// Private Defines ////////////////////////////////////////////

#define TRX_EICRA (0)
// Both interrupts are low-level (Interrupt 1 isn't used).

#define TRX_TRANSACTION_MAX_LENGTH  (33)
// 1 for the instruction, up to 32 for the data.

// Transceiver instruction definitions.

// No-op used for reading the status register.
#define TRX_NOOP_INSTRUCTION                  (0xFF)      
#define TRX_NOOP_TRANSACTION_LENGTH           (1)

// Reads the value of a configuration register.
#define TRX_READ_REGISTER_INSTRUCTION         (0x00)
#define TRX_READ_REGISTER_ADDRESS_MASK        (0x1F)
#define TRX_READ_REGISTER_TRANSACTION_LENGTH  (2)
#define TRX_READ_ADDRESS_TRANSACTION_LENGTH   (5)

// Writes the value of a configuration register.
#define TRX_WRITE_REGISTER_INSTRUCTION        (0x20)
#define TRX_WRITE_REGISTER_ADDRESS_MASK       (0x1F)
#define TRX_WRITE_REGISTER_TRANSACTION_LENGTH (2)
#define TRX_WRITE_ADDRESS_TRANSACTION_LENGTH  (5)

// Writes a payload from the TX buffer.
#define TRX_WRITE_TX_PAYLOAD_INSTRUCTION        (0xA0)
#define TRX_WRITE_TX_PAYLOAD_TRANSACTION_LENGTH (TRX_PAYLOAD_LENGTH + 1)

// Reads a payload from the RX buffer.
#define TRX_READ_RX_PAYLOAD_INSTRUCTION         (0x61)
#define TRX_READ_RX_PAYLOAD_TRANSACTION_LENGTH  (TRX_PAYLOAD_LENGTH + 1)

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

// RX data pipe 0
#define TRX_REGISTER_ADDRESS_RX_PW_P0   (0x11)

// Enable dynaic payload length.
#define TRX_REGISTER_ADDRESS_DYNPD      (0x1C)

// Feature Register
#define TRX_REGISTER_ADDRESS_FEATURE    (0x1D)

// Registers and fields

#define PWR_UP      (1)
#define PRIM_RX     (0)

// Enable all interrupts, enable CRC, 1 byte CRC, power off.
#define TRX_CONFIG_OFF  (0x08)

// Same as above, but powered on and in PTX mode.
#define TRX_CONFIG_TX  (TRX_CONFIG_OFF | _BV(PWR_UP))

// Same as above, but in PRX mode.
#define TRX_CONFIG_RX (TRX_CONFIG_TX | _BV(PRIM_RX))

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

// Number of bytes is data pipes is not relevant for this project, except for
// data pipe 0.
#define TRX_RX_PW_P0  (32)

// FIFO status register are not relevant to this project.

// Dynamic payload length is not used.
#define TRX_DYNPD (0x00)

// Dynamic payload length not used, no payload with acknowledgement, and no
// payloads with no acknowledgement.
#define TRX_FEATURE (0x00)

/////////////////// Static Variable Definitions ////////////////////////////////

trx_status_buffer_t trx_status_buffer;

// The RX address of this data cube.
trx_address_t       trx_this_rx_address;

// The buffer that the rx payload will be read to.
spi_message_element_t *rx_payload_buffer;

/////////////////// Private Function Prototypes ////////////////////////////////

void read_status_buffer();

void update_status_buffer (
  const spi_message_element_t *received_message,
  int received_message_length
);

void uart_transmit_register_value(
  const spi_message_element_t *received_message,
  int received_message_length
);

void uart_transmit_address_value(
  const spi_message_element_t *received_message,
  int received_message_length
);

void write_register(
  spi_message_element_t address,
  spi_message_element_t value
);

void write_address(
  spi_message_element_t register_address,
  trx_address_t         address
);

void write_tx_payload(
  const spi_message_element_t *payload
);

void read_rx_payload(
  spi_message_element_t *buffer
);

void update_rx_payload_buffer(
  const spi_message_element_t *received_message,
  int received_message_length
);

/////////////////// Public Function Bodies /////////////////////////////////////

// Initializes the TRX, including initializing the SPI and any other peripherals
// required.
void trx_initialize(
  trx_address_t rx_address
) {

  trx_this_rx_address = rx_address;

  // Sets the CE pin as an output.
  TRX_CE_DDR |= _BV(TRX_CE_INDEX);
  
  // Set up IRQ pin as an input.
  TRX_IRQ_DDR &= ~_BV(TRX_IRQ_INDEX);

  // Set the IRQ pin's pull-up.
  TRX_IRQ_PORT |= _BV(TRX_IRQ_INDEX);

  // Set the IRQ interrupt logic sense.
  EICRA = TRX_EICRA;

  // Enable the IRQ interrupt.
  //EIMSK |= _BV(TRX_IRQ_INT);

  spi_initialize();

  write_register(
    TRX_REGISTER_ADDRESS_CONFIG,
    TRX_CONFIG_RX
  );

  write_register(
    TRX_REGISTER_ADDRESS_EN_AA,
    TRX_EN_AA
  );

  write_register(
    TRX_REGISTER_ADDRESS_EN_RXADDR,
    TRX_EN_RXADDR
  );

  write_register(
    TRX_REGISTER_ADDRESS_SETUP_AW,
    TRX_SETUP_AW
  );

  write_register(
    TRX_REGISTER_ADDRESS_SETUP_RETR,
    TRX_SETUP_RETR
  );

  write_register(
    TRX_REGISTER_ADDRESS_RF_CH,
    TRX_RF_CH
  );

  write_register(
    TRX_REGISTER_ADDRESS_RF_SETUP,
    TRX_RF_SETUP
  );

  write_register(
    TRX_REGISTER_ADDRESS_DYNPD,
    TRX_DYNPD
  );

  write_register(
    TRX_REGISTER_ADDRESS_FEATURE,
    TRX_FEATURE
  );

  write_address(
    TRX_REGISTER_ADDRESS_RX_ADDR_P0,
    trx_this_rx_address
  );

  uart_transmit_formatted_message("SPI initialization complete!\n\r");
  UART_WAIT_UNTIL_DONE();
  
}

void trx_transmit_payload(
  trx_address_t address,
  trx_payload_element_t *payload,
  int payload_length
) {

  read_status_buffer();

  uart_transmit_formatted_message("Attempting Transmission.\r\n");
  UART_WAIT_UNTIL_DONE();

  // Configure the transceiver as a primary transmitter.
  write_register(
    TRX_REGISTER_ADDRESS_CONFIG,
    TRX_CONFIG_TX
  );
  _delay_ms(500);
  
  // Set the TX address.
  write_address(
    TRX_REGISTER_ADDRESS_TX_ADDR,
    address
  );

  // Set the RX address of Pipe 0.
  write_address(
    TRX_REGISTER_ADDRESS_RX_ADDR_P0,
    address
  );

  // Send the data to the transceiver.
  write_tx_payload(
    payload
  );

  read_status_buffer();

  // Set the CE pin high to begin the transmission.
  TRX_CE_PORT |= _BV(TRX_CE_INDEX);
  _delay_us(20);
  TRX_CE_PORT &= ~_BV(TRX_CE_INDEX);

  // Wait until the transceiver raises the IRQ flag (active low).
  TRX_WAIT_FOR_IRQ();

  read_status_buffer();

  // If the interrupt was a DS (Data Sent) interrupt, the packet has been
  // transmitted successfully.
  if ((trx_status_buffer & _BV(TX_DS)) != 0) {
    uart_transmit_formatted_message(
      "Transmitted payload successfully!\n\r"
    );

    // Clear the interrupt flag
    write_register(
      TRX_REGISTER_ADDRESS_STATUS,
      _BV(TX_DS)
    );

  }

  // If the interrupt was a MAX_RT (Maximum Re-transmissions), the packet was
  // not received by the target.
  else if ((trx_status_buffer & _BV(MAX_RT)) != 0) {
    
    uart_transmit_formatted_message(
      "Failed to transmit payload (ACK timed out).\n\r"
    );

    // Clear the interrupt flag
    write_register(
      TRX_REGISTER_ADDRESS_STATUS,
      _BV(MAX_RT)
    );

  }

  else {
    uart_transmit_formatted_message(
      "Program reached impossible state.\n\r"
    );
  }
  UART_WAIT_UNTIL_DONE();

  // Restore the rx address
  write_address(
    TRX_REGISTER_ADDRESS_RX_ADDR_P0,
    trx_this_rx_address
  );

  // Move back into receive mode.
  write_register(
    TRX_REGISTER_ADDRESS_CONFIG,
    TRX_CONFIG_RX
  );

  SPI_WAIT_UNTIL_DONE();
  UART_WAIT_UNTIL_DONE();

}

// Receives a payload using polling.
int trx_receive_payload(
  trx_payload_element_t *payload_buffer
) {

  // Enable active RX mode.
  TRX_CE_PORT |= _BV(TRX_CE_INDEX);

  // We assume we're already powered on and in receive mode, so we can just
  // wait until something happens.
  TRX_WAIT_FOR_IRQ();

  // Determine which interrupt it was
  read_status_buffer();
  if ((trx_status_buffer & RX_DR) == 0) {
    uart_transmit_formatted_message("Hit an unexpected interrupt while waiting to receive data.\n\r");
    UART_WAIT_UNTIL_DONE();
    return 0;
  }

  // We know it was a data-received interrupt.
  // Read the payload data into the given buffer.
  read_rx_payload(payload_buffer);

  return TRX_PAYLOAD_LENGTH;

}

// Gets the value currently in the status buffer. This is equivalent to what was
// in the transceiver's status register at the beginning of the last SPI
// transaction.
trx_status_buffer_t trx_get_status() {
  return trx_status_buffer;
}

/////////////////// Private function bodies ////////////////////////////////////

void read_status_buffer() {
  spi_message_element_t message[] = {TRX_NOOP_INSTRUCTION};
  spi_begin_transaction(
    message,
    TRX_NOOP_TRANSACTION_LENGTH,
    update_status_buffer
  );
  SPI_WAIT_UNTIL_DONE();
}

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

void uart_transmit_address_value(
  const spi_message_element_t *received_message,
  int received_message_length
) {
  uart_transmit_formatted_message(
    "%02x%02x%02x%02x",
    received_message[1],
    received_message[2],
    received_message[3],
    received_message[4]
  );
}

void write_register(
  spi_message_element_t address,
  spi_message_element_t value
) {
  
  spi_message_element_t message[] = {
    TRX_WRITE_REGISTER_INSTRUCTION | (address & TRX_WRITE_REGISTER_ADDRESS_MASK),
    value
  };
  
  spi_begin_transaction(
    message,
    TRX_WRITE_REGISTER_TRANSACTION_LENGTH,
    NULL
  );
  SPI_WAIT_UNTIL_DONE();

}

void write_address(
  spi_message_element_t register_address,
  trx_address_t         address
) {

  spi_message_element_t message[] = {
    TRX_WRITE_REGISTER_INSTRUCTION | (register_address & TRX_WRITE_REGISTER_ADDRESS_MASK),
    (address >> 24) & 0xFF,
    (address >> 16) & 0xFF,
    (address >> 8)  & 0xFF,
    (address >> 0)  & 0xFF
  };

  spi_begin_transaction(
    message,
    TRX_WRITE_ADDRESS_TRANSACTION_LENGTH,
    NULL
  );
  SPI_WAIT_UNTIL_DONE();

}

void write_tx_payload(
  const spi_message_element_t *payload
) {
  spi_message_element_t message[TRX_WRITE_TX_PAYLOAD_TRANSACTION_LENGTH];
  message[0] = TRX_WRITE_TX_PAYLOAD_INSTRUCTION;
  int i;
  for (i = 0; i < TRX_PAYLOAD_LENGTH; i++) {
    message[i + 1] = payload[i];
  }
  spi_begin_transaction(
    message,
    TRX_WRITE_TX_PAYLOAD_TRANSACTION_LENGTH,
    NULL
  );
  SPI_WAIT_UNTIL_DONE();
}

void read_rx_payload(
  spi_message_element_t *buffer
) {

  rx_payload_buffer = buffer;

  spi_message_element_t message[TRX_READ_RX_PAYLOAD_INSTRUCTION];
  message[0] = TRX_READ_RX_PAYLOAD_INSTRUCTION;
  int i;
  for (i = 0; i < TRX_PAYLOAD_LENGTH; i++){
    message[i + 1] = 0;
  }

  spi_begin_transaction(
    message,
    TRX_READ_RX_PAYLOAD_TRANSACTION_LENGTH,
    update_rx_payload_buffer
  );
  SPI_WAIT_UNTIL_DONE();

}

void update_rx_payload_buffer(
  const spi_message_element_t *received_message,
  int received_message_length
) {
  
  int i;
  for (i = 0; i < received_message_length - 1; i++) {
    rx_payload_buffer[i] = received_message[i + 1];
  }

}

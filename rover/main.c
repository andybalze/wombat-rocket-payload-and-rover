////////////////////////////////////////////////////////////////////////////////
//
// ADC Test Main
//
// Provides a minimal complete test of the ADC functionality. Prints a hello
// world message to an attached UART on reset. Then, responds to incoming UART
// characters with the digital-count value of each ADC channel.
//
// To be a proper test of the ADC, the UART functions must be confirmed to work
// correctly.
//
////////////////////////////////////////////////////////////////////////////////

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "uart.h"
#include "adc.h"

#define KEYSTROKE_ADC0 	('0')
#define KEYSTROKE_ADC1 	('1')
#define KEYSTROKE_ADC2 	('2')
#define KEYSTROKE_ADC3 	('3')
#define KEYSTROKE_ADC4 	('4')
#define KEYSTROKE_ADC5 	('5')
#define KEYSTROKE_ADC6 	('6')
#define KEYSTROKE_ADC7 	('7')
#define KEYSTROKE_TEMP 	('t')
#define KEYSTROKE_REF 	('r')
#define KEYSTROKE_GND 	('g')

static char* restart_message = "Hello World.\n\r";
static int restart_message_length = 15;

static uart_message_element_t received_data;

static char* channel_name_adc0 = "ADC0";
static char* channel_name_adc1 = "ADC1";
static char* channel_name_adc2 = "ADC2";
static char* channel_name_adc3 = "ADC3";
static char* channel_name_adc4 = "ADC4";
static char* channel_name_adc5 = "ADC5";
static char* channel_name_adc6 = "ADC6";
static char* channel_name_adc7 = "ADC7";
static char* channel_name_temp = "Temperature";
static char* channel_name_ref = "Analog Reference";
static char* channel_name_gnd = "Ground Plane";

static char* output_message_format = "%s channel has a value of %u";

#define OUTPUT_MESSAGE_BUFFER_LENGTH (256)
static char output_message_buffer[OUTPUT_MESSAGE_BUFFER_LENGTH];

int main() {

	// Initialized UART.
	uart_initialize();

	// Transmit hello world message.
	uart_transmit_message(restart_message, restart_message_length);

	// Initialize ADC
	adc_initialize();

}

ISR(USART_RX_vect) {
	received_data = UDR0;
	switch (received_data)
	{
	case KEYSTROKE_ADC0:
		
		break;
	
	default:
		break;
	}
}

void transmit_output_message(
	char *channel_name,
	int channel_name_length,
	adc_result_t channel_value
) {

	int output_message_length;
	output_message_length = snprintf(
		output_message_buffer,
		OUTPUT_MESSAGE_BUFFER_LENGTH,
		output_message_format,
		channel_name,
		channel_value
	);

	uart_transmit_message(output_message_buffer, output_message_length);

}

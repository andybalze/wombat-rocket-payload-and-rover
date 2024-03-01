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

#define F_CPU 8000000UL
#include <util/delay.h>

//////////////// Private Defines ///////////////////////////////////////////////

// UART keystrokes that correspond to each ADC channel.
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

#define OUTPUT_MESSAGE_BUFFER_LENGTH (256)

//////////////// Static Variable Definitions ///////////////////////////////////

// Hello world message that is printed to the UART whenever the device is reset.
static char* restart_message = "Hello World.\n\r";
static int restart_message_length = 15;

// The most recent keystroke received from the UART.
static uart_message_element_t received_data;

// The human-readable name of each channel. Used to format the UART output
// message.
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

// The format string for the UART output message.
static char* output_message_format = "%s channel has a value of %u\n\r";

// Buffers the output message string.
static char output_message_buffer[OUTPUT_MESSAGE_BUFFER_LENGTH];

//////////////// Private Function Prototypes ///////////////////////////////////

// Transmits a formatted output message over the UART.
/*void transmit_output_message(
	char *channel_name,
	adc_result_t channel_value
);*/

int main() {

	// Initialized UART.
	uart_initialize();

	// Transmit hello world message.
	// uart_transmit_message(restart_message, restart_message_length);

	// Set up ADC
	ADCSRA |= (
		0
		| _BV(ADEN)
		| _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2)
	);

	ADMUX |= (
		0
		| _BV(REFS0)	// AVCC reference
		| _BV(MUX2)		// Temp channel
	);

	// Start conversion
	ADCSRA |= _BV(ADSC);

	// Wait for conversion to finish
	while((ADCSRA & _BV(ADIF)) == 0);

	// Retreive the result.
	_delay_us(10);
	int result;
	result = (ADCH << 8) + ADCL;

	// Transmit the result over UART.
	char* message_format = "Result: %u\n\r";
	char message[256];
	int message_length;
	message_length = snprintf(
		message,
		256,
		message_format,
		result
	);
	uart_transmit_message(message, message_length);

	while(1);

}

/*
ISR(USART_RX_vect) {
	received_data = UDR0;
	adc_result_t channel_value;
	char *channel_name;
	switch (received_data)
	{
	case KEYSTROKE_ADC0:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC0);
		channel_name  = channel_name_adc0;
		break;
	case KEYSTROKE_ADC1:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC1);
		channel_name  = channel_name_adc1;
		break;
	case KEYSTROKE_ADC2:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC2);
		channel_name  = channel_name_adc2;
		break;
	case KEYSTROKE_ADC3:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC3);
		channel_name  = channel_name_adc3;
		break;
	case KEYSTROKE_ADC4:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC4);
		channel_name  = channel_name_adc4;
		break;
	case KEYSTROKE_ADC5:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC5);
		channel_name  = channel_name_adc5;
		break;
	case KEYSTROKE_ADC6:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC6);
		channel_name  = channel_name_adc6;
		break;
	case KEYSTROKE_ADC7:
		channel_value = adc_get_channel_result(ADC_CHANNEL_ADC7);
		channel_name  = channel_name_adc7;
		break;
	case KEYSTROKE_TEMP:
		channel_value = adc_get_channel_result(ADC_CHANNEL_TEMP);
		channel_name  = channel_name_temp;
		break;
	case KEYSTROKE_REF:
		channel_value = adc_get_channel_result(ADC_CHANNEL_REF);
		channel_name  = channel_name_ref;
		break;
	case KEYSTROKE_GND:
		channel_value = adc_get_channel_result(ADC_CHANNEL_GND);
		channel_name  = channel_name_gnd;
		break;
	default:
		return;
	}
	transmit_output_message(channel_name, channel_value);
}
*/

/*
void transmit_output_message(
	char *channel_name,
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
*/
